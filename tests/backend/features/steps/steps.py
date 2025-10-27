import requests
import json
from behave import step
from tests.common.common_steps import wait_for_backend
from tests.common import common_steps

backend_url = "http://backend:5000"

@step('Backend is available')
def step_impl(context):
    message = "Executing backend component test; scenario: " + context.scenario.name
    wait_for_backend(backend_url, timeout=60, message_to_backend=message)
    requests.request('POST', backend_url + '/stopAllBots')

@step('Request {method} {endpoint} is sent')
def step_impl(context, method, endpoint):
    context.responses.store_response(method, endpoint, requests.get(backend_url + endpoint))

@step('Request {method} {endpoint} is sent with body "{body_as_string}"')
def step_impl(context, method, endpoint, body_as_string):
    headers = {'Content-Type': 'application/json'}
    body_as_object = json.loads(body_as_string)
    response = requests.request(method, backend_url + endpoint, json=body_as_object, headers=headers)
    context.responses.store_response(method, endpoint, response)

@step('Request {method} {endpoint} is sent with query "{query_string}"')
def step_impl(context, method, endpoint, query_string):
    context.responses.store_response(method, endpoint, requests.get(backend_url + endpoint + '?' + query_string))

def valudate_response(context, method, endpoint, status_code, body_as_string):
    actual_response = context.responses.get_last_response(method, endpoint)
    assert actual_response is not None, f"No response found for {method} {endpoint}"
    assert actual_response.status_code == status_code, f"Actual status code: {actual_response.status_code}; expected: {status_code}"
    assert actual_response.text == body_as_string, f"Actual body: {actual_response.text}\nexpected: {body_as_string}"

@step('Response for {method} {endpoint} was received with status code "{status_code:d}" and body "{body_as_string}"')
def step_impl(context, method, endpoint, status_code, body_as_string):
    valudate_response(context, method, endpoint, status_code, body_as_string)

# This version is the step supported when the response body is multi-line:
@step('Response for {method} {endpoint} was received with status code "{status_code:d}" and body')
def step_impl(context, method, endpoint, status_code):
    body_as_string_without_pretty_print = body_as_string = json.dumps(json.loads(context.text), separators=(',', ':'))
    valudate_response(context, method, endpoint, status_code, body_as_string_without_pretty_print)

# This step is used when the response body is expected to be empty - Behave does not support "" param as an empty string
@step('Response for {method} {endpoint} was received with status code "{status_code:d}" and no body')
def step_impl(context, method, endpoint, status_code):
    valudate_response(context, method, endpoint, status_code, "")
