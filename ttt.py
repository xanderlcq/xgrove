import requests
url = 'http://greenwall.elasticbeanstalk.com/api/records/'
params = {'username':'root', 'api_key':'123', 'format':'json'}
data = '{"humidity":"5", "device":{"id":"4"}, "air_pressure":"3.77", "analog_temp":"3", "digital_temp":"3.7", "light_intensity":"0", "water_temp":"3"}'
data = '{"device":{"id":"4"}, "water_temp":"18.94", "analog_temp":"23.24", "light_intensity":"572.00", "air_pressure":"1011.43", "digital_temp":"23.02", "humidity":"23.27"}'
headers = {'content-type': 'application/json'}
requests.post(url, params=params, data=data, headers=headers)