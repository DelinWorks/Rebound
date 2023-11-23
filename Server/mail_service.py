import requests
import sys
url = 'https://api.mailjet.com/v3/send'
headers = {
    'Content-Type': 'application/json; charset=utf-8',
    'Authorization' : 'Basic ZTA0OTA3ZGQyYjgxYTg4NzJkMjEzMzQ4ZDM0NTI2NDA6YThmNjJmYmNjN2QwZDljM2MxMzc3OWZmNThlMWMwMjc='
}

data = '''{ 
    "FromEmail": "mail@delingames.top", 
    "FromName": '$NAME', 
    "Recipients": [{ 
        "Email": '$EMAIL'
    }], 
    "Subject": '$SUBJECT', 
    "Text-part": '$TEXTPART', 
    "Html-part": '$HTMLPART'
}'''

data = data.replace("$NAME", sys.argv[1])
data = data.replace("$EMAIL", sys.argv[2])
data = data.replace("$SUBJECT", sys.argv[3])
data = data.replace("$TEXTPART", '')
data = data.replace("$HTMLPART", sys.argv[4])

data = data.encode('utf8')

response = requests.post(url, headers=headers, data=data)
print(response)
