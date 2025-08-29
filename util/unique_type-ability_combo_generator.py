import json

TYPE_AFFECTING_ABILITIES = {
    "Levitate", "Flash Fire", "Water Absorb", "Volt Absorb",
    "Dry Skin", "Storm Drain", "Lightning Rod", "Thick Fat", "Wonder Guard",
    "Motor Drive", "Heatproof", "Filter", "Solid Rock", "Sap Sipper",
}

def load_pokemon(filename):
    with open(filename, encoding="utf-8") as f:
        return json.load(f)

def combo_key(poke):
    primary = poke.get("primaryType")
    secondary = poke.get("secondaryType")
    # Only keep abilities that affect type effectiveness
    abilities = tuple(sorted(a for a in poke.get("abilities", []) if a in TYPE_AFFECTING_ABILITIES))
    return (primary, secondary, abilities)

def main():
    pokes = load_pokemon("data/pokemon.json")
    combos = {}
    for poke in pokes:
        key = combo_key(poke)
        # Always include the combo, even if no type-affecting ability (store empty list)
        combos[key] = {
            "primaryType": key[0],
            "secondaryType": key[1],
            "abilities": list(key[2])
        }
    # Save as a list
    with open("data/type_ability_combos.json", "w", encoding="utf-8") as f:
        json.dump(list(combos.values()), f, indent=2, ensure_ascii=False)

if __name__ == "__main__":
    main()