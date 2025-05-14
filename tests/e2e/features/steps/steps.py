import requests
from behave import step
from time import sleep
from selenium import webdriver
from selenium.webdriver.common.by import By
from tests.common.common_steps import wait_for_backend
from tests.common.market_service_mock import MarketServiceMock

backend_url = "http://backend:5000"

@step('Backend is available')
def step_impl(context):
    wait_for_backend(backend_url)

@step('Frontend is available')
def step_impl(context):
    selenium_server_url = 'http://selenium:4444'
    frontend_url = 'http://frontend:4200'

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

@step('MarketService mock is running')
def step_impl(context):
    context.marketServiceMock = MarketServiceMock()

    account_info = {
        "balances": [
        {
            "free": "1.234",
            "asset": "BTC"
        },
        {
            "free": "123.4",
            "asset": "ETH"
        }]
    }
    context.marketServiceMock.set_endpoint('/account', 'GET', account_info)

    exchange_info = {
        "symbols": [
        {
            "symbol": "BTCUSDT",
            "baseAsset": "BTC",
            "quoteAsset": "USDT"
        },
        {
            "symbol": "ETHUSDT",
            "baseAsset": "ETH",
            "quoteAsset": "USDT"
        }]
    }
    context.marketServiceMock.set_endpoint('/exchangeInfo', 'GET', exchange_info)

    prices = [
    {
        "price": "80000",
        "symbol": "BTCUSDT"
    },
    {
        "price": "2000",
        "symbol": "ETHUSDT"
    }]
    context.marketServiceMock.set_endpoint('/ticker/price', 'GET', prices)

    context.marketServiceMock.run()

@step('Assets page is opened')
def step_impl(context):
    main_menu = context.webdriver.find_element(By.ID, "main-menu")
    main_menu.click()

    assets_menu_item = context.webdriver.find_element(By.ID, "assets-menu-item")
    assets_menu_item.click()

@step('Assets are presented')
def step_impl(context):
    asset_table_body = context.webdriver.find_element(By.XPATH, "//tbody")
    asset_table_entries = asset_table_body.find_elements(By.TAG_NAME, "tr")

    assert 2 == len(asset_table_entries), f'actual size: {len(asset_table_entries)}'
    
    def assertAssetEntryEqual(tableEntry, expectedSymbol, expectedFreeQuantity, expectedUsdtValue):
        assetSymbolCell = tableEntry.find_element(By.CLASS_NAME, "mat-column-assetSymbol")
        assert expectedSymbol in assetSymbolCell.text, f'actual symbol: {assetSymbolCell.text}'

        freeQuantityCell = tableEntry.find_element(By.CLASS_NAME, "mat-column-freeQuantity")
        assert expectedFreeQuantity in freeQuantityCell.text, f'actual free quantity: {freeQuantityCell.text}'

        usdtValueCell = tableEntry.find_element(By.CLASS_NAME, "mat-column-usdtValue")
        assert expectedUsdtValue in usdtValueCell.text, f'actual USDT value: {usdtValueCell.text}'

    assertAssetEntryEqual(asset_table_entries[0], "BTC", "1.234", "98720.")
    assertAssetEntryEqual(asset_table_entries[1], "ETH", "123.4", "246800.")
