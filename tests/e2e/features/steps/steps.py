import requests
import json
from behave import step, given
from time import sleep
from selenium import webdriver
from tests.common.common_steps import wait_for_backend
from tests.e2e.pages.base_layout import BaseLayout
from tests.e2e.pages.assets_page import AssetsPage
from tests.e2e.pages.orders_page import OrdersPage
from tests.e2e.pages.bots_launch_page import BotsLaunchPage
from tests.e2e.pages.make_order_dialog import MakeOrderDialog
from tests.e2e.pages.backtest_dialog import BacktestDialog
from tests.common import common_steps

backend_url = "http://backend:5000"
selenium_server_url = 'http://selenium:4444'
frontend_url = 'http://frontend:4200'

def wait_for_frontend(context):
    options = webdriver.ChromeOptions()
    options.add_argument("--headless")
    options.add_argument("--disable-gpu")
    options.add_argument("--window-size=1280,1024")
    options.set_capability("goog:loggingPrefs", {"browser": "ALL"})

    for _ in range(60):
        try:
            response = requests.get(frontend_url)
            if response.status_code == 200:
                break
        except requests.ConnectionError:
            sleep(1)
    else:
        raise Exception(f"Could not connect to frontend on {frontend_url}")

    context.webdriver = webdriver.Remote(command_executor=selenium_server_url, options=options)
    context.webdriver.get(frontend_url)

    assert "Algo Trader" in context.webdriver.title, f'Actual title: {context.webdriver.title}'

def setup_pages(context):
    context.base_layout = BaseLayout(context.webdriver)
    context.assets_page = AssetsPage(context.webdriver)
    context.orders_page = OrdersPage(context.webdriver)
    context.bots_launch_page = BotsLaunchPage(context.webdriver)
    context.make_order_dialog = MakeOrderDialog(context.webdriver)
    context.backtest_dialog = BacktestDialog(context.webdriver)

@step('AlgoTrader is running')
def step_impl(context):
    message = "Executing e2e test; scenario: " + context.scenario.name
    wait_for_backend(backend_url, timeout=60, message_to_backend=message)
    requests.request('POST', backend_url + '/stopAllBots')
    wait_for_frontend(context)

    setup_pages(context)

@step('Assets page is opened')
def step_impl(context):
    context.base_layout.navigate_to_assets_page()

@step('Orders page is opened')
def step_impl(context):
    context.base_layout.navigate_to_orders_page()

@step('Bots page is opened')
def step_impl(context):
    context.base_layout.navigate_to_bots_page()

@step('Assets are presented')
def step_impl(context):
    assets = context.assets_page.get_assets_texts()
    assert 2 == len(assets), f'actual number of assets: {len(assets)}'

    def assertAssetContains(tableEntry, expectedSymbol, expectedFreeQuantity, expectedUsdtValue):
        assert expectedSymbol in tableEntry["assetSymbolText"], f'actual symbol text: {tableEntry["assetSymbolText"]}'
        assert expectedFreeQuantity in tableEntry["freeQuantityText"], f'actual free quantity text: {tableEntry["freeQuantityText"]}'
        assert expectedUsdtValue in tableEntry["usdtValueText"], f'actual USDT value text: {tableEntry["usdtValueText"]}'

    assertAssetContains(assets[0], "BTC", "1.234", "98720.")
    assertAssetContains(assets[1], "ETH", "123.4", "246800.")

@step('Make Order dialog is opened for "{assetSymbol}"')
def step_impl(context, assetSymbol):
    context.assets_page.open_make_order_dialog(assetSymbol)

@step('Make Order dialog is filled with quote asset "{quoteAsset}", order side "{orderSide}", amount "{amount}" and confirmed')
def step_impl(context, quoteAsset, orderSide, amount):
    context.make_order_dialog.set_quote_asset(quoteAsset)
    context.make_order_dialog.set_order_side(orderSide)
    context.make_order_dialog.set_amount(amount)

    context.make_order_dialog.click_confirm_button()

@step('Active orders are presented')
def step_impl(context):
    orders = context.orders_page.get_active_orders_texts()
    assert 2 == len(orders), f'actual number of orders: {len(orders)}'

    def assertOrderContains(tableEntry, expectedAssetPair, expectedSide, expectedPrice, expectedOrigQuantity, expectedExecutedQuantity):
        assert expectedAssetPair in tableEntry["assetPairText"], f'actual asset pair text: {tableEntry["assetPairText"]}'
        assert expectedSide in tableEntry["orderSideText"], f'actual order side text: {tableEntry["orderSideText"]}'
        assert expectedPrice in tableEntry["priceText"], f'actual price text: {tableEntry["priceText"]}'
        assert expectedOrigQuantity in tableEntry["origQuantityText"], f'actual original quantity text: {tableEntry["origQuantityText"]}'
        assert expectedExecutedQuantity in tableEntry["executedQuantityText"], f'actual executed quantity text: {tableEntry["executedQuantityText"]}'

    assertOrderContains(orders[0], "BTCUSDT", "Buy", "80000", "0.1", "0.01")
    assertOrderContains(orders[1], "ETHUSDT", "Sell", "2000", "2", "1")

@step('Bot "{bot_name}" is selected')
def step_impl(context, bot_name):
    context.bots_launch_page.select_bot(bot_name.lower())

@step('Rebalancer bot configuration is filled with execution period "{execution_period}", quote asset "{quote_asset}", base assets [{base_assets}] with shares [{base_assets_shares}]')
def step_impl(context, execution_period, quote_asset, base_assets, base_assets_shares):
    context.bots_launch_page.set_execution_period(execution_period)
    context.bots_launch_page.toggle_is_executed_immediately()
    context.bots_launch_page.set_quote_asset(quote_asset)

    base_assets = base_assets.split(',')
    base_assets_shares = base_assets_shares.split(',')
    for single_base_asset, single_base_asset_share in zip(base_assets, base_assets_shares):
        context.bots_launch_page.add_base_asset(
            single_base_asset.strip().replace('"', '').replace("'", ""),
            single_base_asset_share.strip().replace('"', '').replace("'", "")
        )

@step('Bot launch button is clicked')
def step_impl(context):
    context.bots_launch_page.click_launch_bot_button()

@step('Open bot backtest dialog button is clicked')
def step_impl(context):
    context.bots_launch_page.click_open_bot_backtest_dialog_button()

@step('Backtest configuration is filled with transaction fee percent "{fee_percent}", owned assets [{owned_assets}] with amounts [{owned_assets_amounts}] respectively, simulation start date "{start_date}", start time "{start_time}", simulation end date "{end_date}", end time "{end_time}')
def step_impl(context, fee_percent, owned_assets, owned_assets_amounts, start_date, start_time, end_date, end_time):
    context.backtest_dialog.set_fee_percent(fee_percent)

    owned_assets = owned_assets.split(',')
    owned_assets_amounts = owned_assets_amounts.split(',')

    for single_owned_asset, single_owned_asset_amount in zip(owned_assets, owned_assets_amounts):
        context.backtest_dialog.add_owned_asset(
            single_owned_asset.strip().replace('"', '').replace("'", ""),
            single_owned_asset_amount.strip().replace('"', '').replace("'", "")
        )

    context.backtest_dialog.set_simulation_start_date(start_date)
    context.backtest_dialog.set_simulation_start_time(start_time)

    context.backtest_dialog.set_simulation_end_date(end_date)
    context.backtest_dialog.set_simulation_end_time(end_time)


@step('Launch backtest button is clicked')
def step_impl(context):
    context.backtest_dialog.click_launch_backtest_button()

@step('Total profit if held "{total_profit_if_held}" is shown')
def step_impl(context, total_profit_if_held):
    results = context.backtest_dialog.get_backtest_results()
    assert results['total_profit_if_held'] == total_profit_if_held, f'actual total profit: {results["total_profit_if_held"]}'

@step('Total profit "{total_profit_with_trades}" is shown')
def step_impl(context, total_profit_with_trades):
    results = context.backtest_dialog.get_backtest_results()
    assert results['total_profit_with_trades'] == total_profit_with_trades, f'actual total profit: {results["total_profit_with_trades"]}'
