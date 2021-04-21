from requests import get

URLS = {
    "hashes.json": "https://bestdori.com/api/hash/all.json",
    "characters.json": "https://bestdori.com/api/characters/main.2.json",
    "cards.json": "https://bestdori.com/api/cards/all.5.json"
}
for filename, url in URLS.items():
    response = get(url)
    with open(filename, "w") as f:
        f.write(response.text)
