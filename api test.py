import requests
url = 'http://greenwall.elasticbeanstalk.com/api/log/'
params = {'username':'root', 'api_key':'123', 'format':'json'}
data =  '{"moisture": "3", "addWater":"3", "light":"3", "lightOn":"0", "humidity":"3", "ph":"3", "mass":"3", "sound":"3", "error":"3", "plant":{"id":"3"}}'
headers = {'content-type': 'application/json'}
requests.post(url, params=params, data=data, headers=headers)