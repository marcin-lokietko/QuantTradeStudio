def after_scenario(context, scenario):
    if hasattr(context, 'marketServiceMock'):
        context.marketServiceMock.stop()
