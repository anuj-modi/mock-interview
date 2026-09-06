"""
Service that generates link previews (title, description, thumbnail)
for URLs users paste into the chat product, similar to how Slack or
iMessage shows a preview card for a shared link.
"""

import requests
from flask import Flask, request, jsonify
from bs4 import BeautifulSoup

app = Flask(__name__)


def fetch_metadata(url: str) -> dict:
    response = requests.get(url, timeout=5)
    soup = BeautifulSoup(response.text, "html.parser")

    title_tag = soup.find("title")
    description_tag = soup.find("meta", attrs={"name": "description"})

    return {
        "title": title_tag.text if title_tag else None,
        "description": description_tag["content"] if description_tag else None,
        "status_code": response.status_code,
    }


@app.route("/preview")
def preview():
    url = request.args.get("url", "")
    if not url.startswith("http://") and not url.startswith("https://"):
        return jsonify({"error": "url must start with http:// or https://"}), 400

    try:
        metadata = fetch_metadata(url)
    except requests.RequestException as e:
        return jsonify({"error": str(e)}), 502

    return jsonify(metadata)


@app.route("/webhook/validate")
def validate_webhook():
    """
    Lets a customer register a webhook URL; we ping it once to confirm
    it's reachable before saving it to their integration settings.
    """
    callback_url = request.args.get("callback_url", "")
    try:
        resp = requests.get(callback_url, timeout=3)
        reachable = resp.status_code < 500
    except requests.RequestException:
        reachable = False

    return jsonify({"reachable": reachable})


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5001)
