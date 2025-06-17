import threading
import os
from flask import Flask, jsonify, request
from werkzeug.serving import make_server

class MarketServiceMock:
    def __init__(self):
        self.app = Flask('market_service_mock', root_path=os.getcwd())
        self._server = None
        self._thread = None
        self._received_requests = []

    def set_endpoint(self, endpoint, method, response=None, status_code=200):
        print(f"Setting up mock endpoint: |{method}| |{endpoint}| -> {response}")

        # Flask requires unique endpoint name for each route
        endpoint_name = f"{method}_{endpoint}".replace('/', '_')
        def callback():
            query_dict = request.args.to_dict()
            request_body = request.get_data(as_text=True)

            self._received_requests.append({
                'endpoint': endpoint,
                'method': method,
                'query_dict': query_dict,
                'request_body': request_body
            })

            print(f'MarketSeviceMock called on {method} {endpoint} request query: {query_dict}; request body: {request_body}; mocked response: {str(response)}' )
            if response is None:
                return '', status_code
            return jsonify(response), status_code

        self.app.add_url_rule(endpoint, view_func=callback, methods=[method], endpoint=endpoint_name)

    def assert_endpoint_invoked_with(self, method, endpoint, expected_query=None, expected_body=None):
        for request in self._received_requests:
            if request['method'] == method and request['endpoint'] == endpoint:
                # Check that all expected query params are present in the request (note there may be more params in the actual request)
                is_query_ok = expected_query is None\
                    or all(pair in request['query_dict'].items() for pair in expected_query.items())

                is_body_ok = expected_body is None\
                    or request['request_body'] == expected_body

                if is_query_ok and is_body_ok:
                    return

        raise AssertionError(f"Endpoint call assertion failed for: {method} {endpoint} expected_query={expected_query}, expected_body={expected_body}. Actual received requests: {self._received_requests}")

    #"0.0.0.0" binds to all available network interfaces - needed for Docker bridge network
    def run(self, host="0.0.0.0", port=5001):
        self._server = make_server(host, port, self.app)
        self._thread = threading.Thread(target=self._server.serve_forever)
        self._thread.start()

    def stop(self):
        if self._server:
            self._server.shutdown()
            self._thread.join()

def set_default_config(market_service_mock):
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
    market_service_mock.set_endpoint('/account', 'GET', account_info)

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
        },
        {
            "symbol": "ETHBTC",
            "baseAsset": "ETH",
            "quoteAsset": "BTC"
        }]
    }
    market_service_mock.set_endpoint('/exchangeInfo', 'GET', exchange_info)

    prices = [
        {
            "price": "80000",
            "symbol": "BTCUSDT"
        },
        {
            "price": "2000",
            "symbol": "ETHUSDT"
        },
        {
            "price": "0.024",
            "symbol": "ETHBTC"
        }
    ]
    market_service_mock.set_endpoint('/ticker/price', 'GET', prices)

    market_service_mock.set_endpoint('/order', 'POST')

    open_orders = [
        {
            "symbol": "BTCUSDT",
            "orderId": 1,
            "price": "80000",
            "origQty": "0.1",
            "executedQty": "0.01",
            "side": "BUY",
        },
        {
            "symbol": "ETHUSDT",
            "orderId": 2,
            "price": "2000",
            "origQty": "2",
            "executedQty": "1",
            "side": "SELL",
        }
    ]
    market_service_mock.set_endpoint('/openOrders', 'GET', open_orders)
