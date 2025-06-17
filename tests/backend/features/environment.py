from tests.backend.utils.backend_responses import BackendResponses

def before_scenario(context, scenario):
    context.responses = BackendResponses()

def after_scenario(context, scenario):
    if hasattr(context, 'market_service_mock'):
        context.market_service_mock.stop()

    if hasattr(context, 'webdriver'):
        context.webdriver.quit()

