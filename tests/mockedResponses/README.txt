The files in this directory define request and response in the following format:
request 1 query dict
request 1 body
response 1 body
response 1 status code

request 2 query dict
request 2 body
response_body2.json
response 2 status code

request 3 query dict
request 3 body
response 3
response 3 status code

During runtime, each actual request is compared to the mocked request query and body pairs from top to bottom. Every mocked request query and body is defined as a set of key-value pairs. An actual request matches a mocked request if all the mocked key-value pairs are present in the actual request query and body. This means that "{}" will be matched with any request.

The request is always defined inline. The response can be defined inline or defined in a separate .json file.
