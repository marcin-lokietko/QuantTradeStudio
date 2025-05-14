import threading
import os
from flask import Flask, jsonify
from werkzeug.serving import make_server

class MarketServiceMock:
    def __init__(self):
        self.app = Flask('market_service_mock', root_path=os.getcwd())
        self._server = None
        self._thread = None
        # self.setup_routes()

    def set_endpoint(self, endpoint, method, response):
        print(f"Setting up mock endpoint: {method} {endpoint} -> {response}")

        # Flask requires unique endpoint name for each route
        endpoint_name = f"{method}_{endpoint}".replace('/', '_')
        def callback():
            print('MarketSeviceMock /ticker/price returns: ' + str(response))
            return jsonify(response)

        self.app.add_url_rule(endpoint, view_func=callback, methods=[method], endpoint=endpoint_name)

    #"0.0.0.0" binds to all available network interfaces - needed for Docker bridge network
    def run(self, host="0.0.0.0", port=5001):
        self._server = make_server(host, port, self.app)
        self._thread = threading.Thread(target=self._server.serve_forever)
        self._thread.start()

    def stop(self):
        if self._server:
            self._server.shutdown()
            self._thread.join()
