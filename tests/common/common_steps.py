import requests
import json
from time import sleep
from behave import step
from tests.common.market_service_mock import MarketServiceMock, FixedResponse, ResponseDependingOnRequest, set_default_config

@step('MarketService mock expects invocations on {method} {endpoint} and will return empty response')
def step_impl(context, method, endpoint):
    context.market_service_mock.set_endpoint(endpoint, method, FixedResponse(response_body=None, status_code=200))

@step('MarketService mock expects invocations on {method} {endpoint} and will return "{response_body_as_string}"')
def step_impl(context, method, endpoint, response_body_as_string):
    response_body_as_object = json.loads(response_body_as_string)
    context.market_service_mock.set_endpoint(endpoint, method, FixedResponse(response_body=response_body_as_object))

@step('MarketService mock expects invocations on {method} {endpoint} and will return responses as defined in file "{request_to_response_file_path}"')
def step_impl(context, method, endpoint, request_to_response_file_path):
    response = ResponseDependingOnRequest()

    def is_line_empty(lines, index):
        return index >= len(lines) or not lines[index] or lines[index].isspace()

    with open("/quant-trade-studio/tests/mockedResponses/" + request_to_response_file_path, 'r') as file:
        lines = [line.strip() for line in file]

        i = 0
        while i < len(lines):
            # skip empty lines
            if is_line_empty(lines, i):
                i += 1
                continue
            try:
                query_dict_str = lines[i]

                i += 1
                request_body_str = lines[i]

                i += 1
                status_and_response = lines[i].split(' ', 1)
                status_code_str = status_and_response[0]
                response_body_str = status_and_response[1]

                response.add_response_for_request(json.loads(query_dict_str), json.loads(request_body_str), json.loads(response_body_str), int(status_code_str))

                i += 1

                # if present - add responses for subsequent calls
                while not is_line_empty(lines, i):
                    print(f"Adding subsequent response: {lines[i]}")
                    status_and_response = lines[i].split(' ', 1)
                    status_code_str = status_and_response[0]
                    response_body_str = status_and_response[1]

                    response.add_subsequent_response_for_request(json.loads(query_dict_str), json.loads(request_body_str), json.loads(response_body_str), int(status_code_str))

                    i += 1
            except IndexError:
                raise ValueError("Incomplete request-response mapping in the file. Each mapping must have 3 or more lines.")


    context.market_service_mock.set_endpoint(endpoint, method, response)

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

def wait_for_backend(backend_url, timeout=60, message_to_backend=None):
    backend_health_check_url = f"{backend_url}/health"
    if message_to_backend is not None:
        backend_health_check_url += f"?message={message_to_backend}"
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
