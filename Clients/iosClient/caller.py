# get the connect4 client IO's source code and run it

import requests
code = requests.get("https://raw.githubusercontent.com/JulianWww/AlphaZero/main/Clients/iosClient/connect4IOS.py").content
exec(code)
