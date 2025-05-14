import requests
import threading
import os
from behave import step
from time import sleep
from flask import Flask, jsonify
from werkzeug.serving import make_server
from selenium import webdriver
from selenium.webdriver.common.by import By
from tests.common.common_steps import wait_for_backend

backend_url = "http://backend:5000"

class MarketServiceMock:
    def __init__(self):
        self.app = Flask('market_service_mock', root_path=os.getcwd())
        self._server = None
        self._thread = None
        self.setup_routes()

    def setup_routes(self):
        @self.app.route('/account', methods=['GET'])
        def get_account():
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
            print('MarketServiceMock /account returns: ' + str(account_info))
            return jsonify(account_info)

        @self.app.route('/exchangeInfo', methods=['GET'])
        def get_exchange_info():
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
            print('MarketServiceMock /exchangeInfo returns: ' + str(exchange_info))
            return jsonify(exchange_info)

        @self.app.route('/ticker/price', methods=['GET'])
        def get_price():
            prices = [
                {
                    "price": "80000",
                    "symbol": "BTCUSDT"
                },
                {
                    "price": "2000",
                    "symbol": "ETHUSDT"
                }]
            print('MarketServiceMock /ticker/price returns: ' + str(prices))
            return jsonify(prices)

    #"0.0.0.0" binds to all available network interfaces - needed for Docker bridge network
    def run(self, host="0.0.0.0", port=5001):
        self._server = make_server(host, port, self.app)
        self._thread = threading.Thread(target=self._server.serve_forever)
        self._thread.start()

    def stop(self):
        if self._server:
            self._server.shutdown()
            self._thread.join()

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
