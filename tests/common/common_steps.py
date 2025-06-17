import requests
import json
from time import sleep
from behave import step
from tests.common.market_service_mock import MarketServiceMock, set_default_config

@step('MarketService mock expects invocations on {method} {endpoint} and will return empty response')
def step_impl(context, method, endpoint):
    context.market_service_mock.set_endpoint(endpoint, method)

@step('MarketService mock expects invocations on {method} {endpoint} and will return "{response_body_as_string}"')
def step_impl(context, method, endpoint, response_body_as_string):
    response_body_as_object = json.loads(response_body_as_string)
    context.market_service_mock.set_endpoint(endpoint, method, response_body_as_object)

@step('MarketService method {method} of endpoint {endpoint} has been invoked with query params "{expected_query_string_as_dict}"')
def step_impl(context, method, endpoint, expected_query_string_as_dict):
    context.market_service_mock.assert_endpoint_invoked_with(
        method, endpoint, json.loads(expected_query_string_as_dict)
    )

@step('MarketService mock is running with default configuration')
def step_impl(context):
    context.market_service_mock = MarketServiceMock()
    set_default_config(context.market_service_mock)
    context.market_service_mock.run()

@step('MarketService mock is running')
def step_MarketService_mock_is_running(context):
    context.market_service_mock = MarketServiceMock()
    context.market_service_mock.run()

@step('MarketService mock is reset')
def step_impl(context):
    context.market_service_mock.stop()
    context.market_service_mock = None
    step_MarketService_mock_is_running(context)

@step('System runs for {num_seconds:g} sec')
def step_impl(context, num_seconds):
    sleep(num_seconds)

def wait_for_backend(backend_url, timeout=60):
    backend_health_check_url = f"{backend_url}/health"
    for _ in range(timeout):
        try:
            response = requests.get(backend_health_check_url)
            if response.status_code == 200:
                print(f"Backend is available at {backend_health_check_url}.")
                return
        except requests.ConnectionError:
            pass
        sleep(1)
    
    raise Exception(f"Backend is not available at {backend_health_check_url}")
