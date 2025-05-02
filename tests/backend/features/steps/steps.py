import requests
from behave import step
from time import sleep

@step('Backend is available')
def step_impl(context):
    sleep(3)

@step('Order request is sent')
def step_impl(context):
    context.response = requests.get("http://backend:5000/hello")

@step('Order response is received')
def step_impl(context):
    assert context.response.text == 'Hello world from backend', f"Actual response: {context.response.text}"
