The files in this directory define request and response in the following format:
request_1_query_dict
request_1_body
response_1_status_code_1 response_1_body_1
response_1_status_code_2 response_1_body_2
response_1_status_code_3 response_1_body_3

request_2_query_dict
request_2_body
response_2_status_code response_body2.json


request_3_query_dict
request_3_body
response_3_status_code response_3_body


During runtime, each actual request is compared to the mocked request query and body pairs from top to bottom. Every mocked request query and body is defined as a set of key-value pairs. An actual request matches a mocked request if all the mocked key-value pairs are present in the actual request query and body. This means that "{}" will be matched with any request.

If there are multiple response code+body defined for single request query+body, then the subsequent response code+body will be used for subsequent matched requests. When all intermediate response code+body are used, the last response code+body will be used for all the remianing matched requests.

The request is always defined inline. The response can be defined inline or defined in a separate .json file.

The query dict only allows key:string pairs. In case something more complex is sent, e.g. &symbols=["ETHUSDT","BTCUSDT"], it still has to be written in a form of key:string, so: {"symbols":"[\"ETHUSDT\",\"BTCUSDT\"]"}