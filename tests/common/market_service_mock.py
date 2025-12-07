import threading
import os
from flask import Flask, jsonify, request
from werkzeug.serving import make_server

class FixedResponse:
    def __init__(self, response_body=None, status_code=200):
        self.response_body = response_body
        self.status_code = status_code

    def get_response(self, request_query_dict, request_body):
        return self.response_body, self.status_code

    def __repr__(self):
        return f"FixedResponse(status={self.status_code}, body={self.response_body})"

class ResponseDependingOnRequest:
    def __init__(self):
        self.mocked_response_list = []

    def add_response_for_request(self, request_query_dict, request_body_dict, response_body, response_status_code):
        self.mocked_response_list.append({
            "request_query_dict": request_query_dict,
            "request_body_dict": request_body_dict,
            "responses": [
                {
                    "response_body": response_body,
                    "response_status_code": response_status_code
                },
            ],
            "current_response_index": 0
        })

    def add_subsequent_response_for_request(self, request_query_dict, request_body_dict, response_body, response_status_code):
        # Find the existing mocked response for the EXACT given request (query + body)
        for mocked_response in self.mocked_response_list:
            if mocked_response["request_query_dict"] == request_query_dict and \
               mocked_response["request_body_dict"] == request_body_dict:
                mocked_response["responses"].append({
                    "response_body": response_body,
                    "response_status_code": response_status_code
                })
                return
        raise ValueError("No existing mocked response found for the given request to add subsequent response")

    def get_response(self, actual_request_query_dict, actual_request_body_dict):
        print(f"Looking for mocked response for request query: {actual_request_query_dict}; request body: {actual_request_body_dict}")

        for mocked_response in self.mocked_response_list:
            query_match = not mocked_response["request_query_dict"] or \
                             all(mocked_key in actual_request_query_dict and actual_request_query_dict[mocked_key] == mocked_value
                             for mocked_key, mocked_value in mocked_response["request_query_dict"].items())

            body_match = not mocked_response["request_body_dict"] or \
                             all(mocked_key in actual_request_body_dict and actual_request_body_dict[mocked_key] == mocked_value
                             for mocked_key, mocked_value in mocked_response["request_body_dict"].items())

            if query_match and body_match:
                response_index = mocked_response["current_response_index"]
                # stay on the last response if exceeded
                if response_index >= len(mocked_response["responses"]):
                    response_index = len(mocked_response["responses"]) - 1

                body = mocked_response["responses"][response_index]["response_body"]
                status = mocked_response["responses"][response_index]["response_status_code"]
                mocked_response["current_response_index"] += 1

                print(f"Found mocked response: body={body} status={status} (response index: {response_index})")
                return body, status

        return "Fatal: request is not expected by test scenario", 404

    def __repr__(self):
        summary = []
        for r in self.mocked_response_list:
            responses_summary = ''
            for resp in r['responses']:
                responses_summary += f"[status={resp['response_status_code']}, body={resp['response_body']}], "
            responses_summary = responses_summary.rstrip(', ')
            summary.append(f"Query: {r['request_query_dict']}, Body: {r['request_body_dict']} -> Responses: {responses_summary}")

        return f"ResponseDependingOnRequest({', '.join(summary)})"


class MarketServiceMock:
    def __init__(self):
        self.app = Flask('market_service_mock', root_path=os.getcwd())
        self._server = None
        self._thread = None
        self._received_requests = []

    def set_endpoint(self, endpoint, method, response):
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

            response_body, response_status_code = response.get_response(query_dict, request_body)

            print(f'MarketSeviceMock called on {method} {endpoint} request query: {query_dict}; request body: {request_body}; mocked response: {response_status_code} {response_body} ' )
            if response_body is None:
                return '', response_status_code
            return jsonify(response_body), response_status_code

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
    market_service_mock.set_endpoint('/account', 'GET', FixedResponse(account_info))

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
    market_service_mock.set_endpoint('/exchangeInfo', 'GET', FixedResponse(exchange_info))

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
    market_service_mock.set_endpoint('/ticker/price', 'GET', FixedResponse(prices))

    market_service_mock.set_endpoint('/order', 'POST', FixedResponse())

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
    market_service_mock.set_endpoint('/openOrders', 'GET', FixedResponse(open_orders))
