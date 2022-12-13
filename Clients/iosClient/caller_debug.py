#debug version of the caller

import requests
code = requests.get("https://raw.githubusercontent.com/JulianWww/AlphaZero/main/Clients/iosClient/connect4IOS_d.py").content
exec(code)
