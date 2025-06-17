class BackendResponses:
    def __init__(self):
        self.responses = {}

    def store_response(self, method, endpoint, response):
        key = (method, endpoint)
        self.responses.setdefault(key, []).append(response)

    def get_last_response(self, method, endpoint):
        key = (method, endpoint)
        if key in self.responses and self.responses[key]:
            return self.responses[key][-1]
        return None
