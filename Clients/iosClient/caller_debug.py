import requests
code = requests.get("https://github.com/JulianWww/AlphaZero/blob/main/Clients/iosClient/connect4IOS.py").content
exec(code)
