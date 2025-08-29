import requests
import json
import time
import os

API_BASE = "https://pokeapi.co/api/v2"
PAGE_SIZE = 200  # be polite

def get_total_pokemon():
    resp = requests.get(f"{API_BASE}/pokemon?limit=1")
    resp.raise_for_status()
    return resp.json()["count"]
    # return 100 # For debugging, only fetch specific amount

def iter_all_pokemon():
    total = get_total_pokemon()
    fetched = 0
    offset = 0
    while fetched < total:
        limit = min(PAGE_SIZE, total - fetched) # Only fetch up to the remaining needed
        resp = requests.get(f"{API_BASE}/pokemon", params={"limit": limit, "offset": offset})
        resp.raise_for_status()
        results = resp.json()["results"]
        if not results:
            break
        for r in results:
            fetched += 1
            yield fetched, total, r
            if fetched >= total:
                break
        offset += limit
        time.sleep(0.25)  # be nice to the api

def get_pokemon_data(poke_url):
    resp = requests.get(poke_url)
    resp.raise_for_status()
    data = resp.json()
    dex_id = data["id"]  # National Dex number
    name = data["name"].capitalize()
    types = [t["type"]["name"].capitalize() for t in sorted(data["types"], key=lambda x: x["slot"])]
    abilities = [a["ability"]["name"].replace("-", " ").capitalize() for a in data["abilities"]]
    entry = {
        "dexNo": dex_id,
        "name": name,
        "primaryType": types[0],
        "abilities": abilities
    }
    if len(types) > 1:
        entry["secondaryType"] = types[1]
    return entry

# MAIN
print("Starting scrape from PokeAPI...", flush=True)
pokemon_list = []
os.makedirs("docker_out", exist_ok=True)

for idx, total, poke in iter_all_pokemon():
    if idx % 5 == 0 or idx == total:
        pct = (idx / total) * 100
        print(f"{idx}/{total} ({pct:.1f}%) Pokémon processed.", flush=True)
    try:
        entry = get_pokemon_data(poke["url"])
        pokemon_list.append(entry)
    except Exception as e:
        print(f"WARNING: Failed to fetch {poke['name']}: {e}", flush=True)
    time.sleep(0.5)  # be nice to the api

with open("docker_out/allPokemon.json", "w", encoding="utf-8") as f:
    f.write('[\n')
    for i, entry in enumerate(pokemon_list):
        line = json.dumps(entry, ensure_ascii=False)
        if i < len(pokemon_list) - 1:
            line += ','
        f.write(line + '\n')
    f.write(']\n')

print("Output file written. cya")