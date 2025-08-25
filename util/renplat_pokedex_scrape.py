import requests
from bs4 import BeautifulSoup
import json
import sys
import re
import time

# USAGE: docker build -f util/Dockerfile.scrape -t renplat_pokedex_scraper . && docker run --rm -v ${PWD}/docker_out:/app/docker_out renplat_pokedex_scraper

def parse_types(soup):
    # Try to find a <tr> with a <td> containing "Complete" and type images
    for tr in soup.find_all("tr"):
        tds = tr.find_all("td")
        if len(tds) >= 2 and tds[0].text.strip().lower() == "complete":
            type_imgs = tds[1].find_all("img", src=lambda s: s and "/types/" in s)
            if type_imgs:
                # delim on "/", get the last field, remove ".png", and capitalize
                return [img["src"].split("/")[-1].replace(".png", "").capitalize() for img in type_imgs]
            # If no type images found (this is an ability row), continue to fallback

    # Fallback: first <td> with type images (for pokemon with no complete/classic distinction)
    for td in soup.find_all("td"):
        type_imgs = td.find_all("img", src=lambda s: s and "/types/" in s)
        if type_imgs:
            return [img["src"].split("/")[-1].replace(".png", "").capitalize() for img in type_imgs]
    
    # If no types are found, return an empty list
    return []


def parse_abilities(soup):
    # Try to find a <tr> with a <td> containing "Complete" and <abbr> abilities
    for tr in soup.find_all("tr"):
        tds = tr.find_all("td")
        if len(tds) >= 2 and tds[0].text.strip().lower() == "complete":
            abbrs = tds[1].find_all("abbr")
            if abbrs:
                return [abbr.text.strip() for abbr in abbrs]
            # If no abilities found (this is a type row), continue to fallback

    # Fallback: first <td> with <abbr> abilities (for pokemon with no complete/classic distinction)
    for td in soup.find_all("td"):
        abbrs = td.find_all("abbr")
        if abbrs:
            return [abbr.text.strip() for abbr in abbrs]
        
    # Minun case: look for <h2 id="ability"> and parse the next <p>
    ability_h2 = soup.find("h2", id="ability")
    if ability_h2:
        p = ability_h2.find_next_sibling("p")
        if p: 
            abbrs = p.find_all("abbr")
            if abbrs:
                return [abbr.text.strip() for abbr in abbrs]

    # If no abilities are found, return an empty list
    return []

print("Starting scrape...", flush=True)
pokemon_list = []
for num in range(1, 493+1):
    if num % 50 == 0:
        print(f"Processed {num:03d}.", flush=True)

    url = f"https://fredericdlugi.github.io/platinum-renegade-wiki/pokemons/{num:03d}/"
    # print(f"Fetching \"{url}\"...", flush=True)
    response = requests.get(url)
    if response.status_code != 200:
        print(f"ERROR: Failed to fetch \"{url}\", status code: {response.status_code}!")
        sys.exit(1)
    
    soup = BeautifulSoup(response.text, 'html.parser')

    h1 = soup.select_one('h1')
    if not h1:
        print(f"ERROR: Failed to find name <h1> for {num:03d}!")
        sys.exit(1)
    raw_name = h1.text.strip()
    match = re.search(r'\d+\s*-\s*([A-Za-z\'\-\. ]+)', raw_name)
    if match:
        name = match.group(1).strip()
    else:
        # fallback: remove any trailing non-letter characters
        name = re.sub(r'^[\d\s\-]+|[^\w\s\'\-\.]+$', '', raw_name).strip()

    types = parse_types(soup)
    if not types:
        print(f"WARNING: Failed to find types for {name}!", flush=True)

    abilities = parse_abilities(soup)
    if not abilities:
        print(f"WARNING: Failed to find abilities for {name}!", flush=True)

    entry = {
        "name": name,
        "primaryType": types[0],
        "abilities": abilities
    }
    if len(types) > 1:
        entry["secondaryType"] = types[1]
    pokemon_list.append(entry)
    # print("Entry added.", flush=True)
    time.sleep(1)

with open("docker_out/allPokemon.json", "w", encoding="utf-8") as f:
    json.dump(pokemon_list, f, ensure_ascii=False, indent=4)

print("Output file written. Goodbye.")
