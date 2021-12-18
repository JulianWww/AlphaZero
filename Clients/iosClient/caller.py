import requests
code = requests.get("http://wandhoven.ddns.net/code/AlphaZero/connect4IOS.py").content
exec(code)
