import requests
from behave import step
from tests.common.common_steps import wait_for_backend
from tests.common.market_service_mock import MarketServiceMock

backend_url = "http://backend:5000"

@step('Backend is available')
def step_impl(context):
    wait_for_backend(backend_url)

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

@step('GET /assets request is sent')
def step_impl(context):
    context.response = requests.get(backend_url + "/assets")

@step('GET /assets response is valid')
def step_impl(context):
    assert context.response.text == r'[{"assetSymbol":"BTC","freeQuantity":"1.234","usdtValue":"98720.000000"},{"assetSymbol":"ETH","freeQuantity":"123.4","usdtValue":"246800.000000"}]',\
        f"Actual response: {context.response.text}"

