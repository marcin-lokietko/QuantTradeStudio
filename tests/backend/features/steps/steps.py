import requests
import threading
import os
from behave import step
from time import sleep
from flask import Flask, jsonify
from werkzeug.serving import make_server

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
                    "symbol": "BTC"
                },
                {
                    "price": "2000",
                    "symbol": "ETH"
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
    sleep(5)

@step('MarketService mock is running')
def step_impl(context):
    context.marketServiceMock = MarketServiceMock()
    context.marketServiceMock.run()

@step('GET /assets request is sent')
def step_impl(context):
    context.response = requests.get("http://backend:5000/assets")

@step('GET /assets response is valid')
def step_impl(context):
    assert context.response.text == r'[{"assetSymbol":"BTC","freeQuantity":"1.234","usdtValue":""},{"assetSymbol":"ETH","freeQuantity":"123.4","usdtValue":""}]',\
        f"Actual response: {context.response.text}"

