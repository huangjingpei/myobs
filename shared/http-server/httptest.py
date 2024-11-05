import requests
import time

def send_post_requests(url, data, interval):
    with requests.Session() as session:
        while True:
            try:
                response = session.post(url, json=data, verify=False)
                print('Status Code:', response.status_code)
                print('Response:', response.json())
            except requests.exceptions.SSLError as ssl_err:
                print('SSL Error:', ssl_err)
            except requests.exceptions.RequestException as req_err:
                print('Request Error:', req_err)

            time.sleep(interval)

if __name__ == "__main__":
    url = "http://127.0.0.1:7979/biz/danmaku/live/msg"
    data = {
        "key1": "value1",
        "key2": "value2"
    }
    interval = 5

    send_post_requests(url, data, interval)
