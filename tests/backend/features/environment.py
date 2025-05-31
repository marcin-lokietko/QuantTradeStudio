def after_scenario(context, scenario):
    if hasattr(context, 'market_service_mock'):
        context.market_service_mock.stop()

    if hasattr(context, 'webdriver'):
        context.webdriver.quit()

