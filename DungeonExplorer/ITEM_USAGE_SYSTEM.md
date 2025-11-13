# Item Usage System - Complete

## Overview
Player can now use items from inventory with full action support including healing, equipment, buffs, and special effects.

## Features Implemented

### 1. New Inventory System
- **LinkedList<ItemNew>** - Stores items with full metadata (action, rarity, type)
- **Equipment Slots** - Separate pointers for equipped weapon and armor
- **Backwards Compatible** - Old Item system still works alongside new system

### 2. Item Actions Supported

#### Heal (`action.kind = "heal"`)
- Restores HP based on `action.params.amount`
- Example: Potion (+50 HP), Elixir (+100 HP)
- Consumable items removed after use
- Checks if already at max health

#### Equip (`action.kind = "equip"`)
- **Weapons**: Add `attack_bonus` to player attack stat
- **Armor**: Add `defense_bonus` to player defense stat
- Auto-unequips old equipment and restores previous stats
- Equipment removed from inventory when equipped
- Example: Iron Sword (+15 attack), Shield (+10 defense)

#### Buff (`action.kind = "buff"`)
- Permanent stat increases
- Supports `health_bonus` (increases max HP)
- Supports `mana_bonus` (increases max mana)
- Consumable buffs removed after use

#### Use (`action.kind = "use"`)
- Special effects (smoke bomb, teleport, etc.)
- Effect stored in `action.params.effect`
- Utility items removed after use

#### Attack (`action.kind = "attack"`)
- Attack items (bombs, throwables)
- Removed after use

### 3. Player Methods

```cpp
// Add item to new inventory system
void addItemNew(const ItemNew& item);

// Remove item by ID
bool removeItemNew(const std::string& itemId);

// Use item by ID - applies action and updates stats
bool useItem(const std::string& itemId);

// Get new inventory
const LinkedList<ItemNew>& getInventoryNew() const;
```

### 4. Item Usage Flow

1. **Pickup**: Items from loot/shop added via `player->addItemNew(item)`
2. **Storage**: Items stored in `LinkedList<ItemNew> inventoryNew`
3. **Usage**: Press **U key** to use first item in inventory
4. **Action**: `useItem()` method:
   - Finds item by ID
   - Applies action based on `action.kind`
   - Updates player stats (HP, attack, defense, etc.)
   - Removes consumable items
   - Logs all actions to console

### 5. Key Bindings

- **E** - Pick up loot from ground
- **P** - Open/close shop
- **U** - Use first item in inventory
- **I** - Toggle inventory display (existing)

### 6. Integration Points

#### Game.cpp
- Loot pickup now uses `addItemNew()` instead of old Item format
- U key handler calls `player->useItem(firstItemId)`

#### Shop.cpp
- Purchase now uses `addItemNew()` instead of old Item format
- Shop items properly integrate with new system

#### Player.cpp
- Constructor initializes equipment pointers to nullptr
- `useItem()` handles all 5 action types
- Equipment management with auto-unequip
- Full debug logging for all item actions

## Console Output Examples

```
[Player] Added Potion to inventory (new system)
[DEBUG] Using item: Potion (type: consumable, action: heal)
[DEBUG] Player HP 100/100 (healed 50)
[Player] Used Potion - healed 50 HP!
[DSA-LinkedList] Removed element. List size: 0
[Player] Removed Potion from inventory

[Shop] Purchased Iron Sword for 100 gold
[Player] Added Iron Sword to inventory (new system)
[DEBUG] Using item: Iron Sword (type: weapon, action: equip)
[Player] Equipped Iron Sword (+15 attack). Total attack: 30
[DSA-LinkedList] Removed element. List size: 0
[Player] Removed Iron Sword from inventory
```

## Testing Checklist

✅ Heal items restore HP correctly
✅ Heal items don't work at max HP
✅ Weapon equip increases attack stat
✅ Armor equip increases defense stat
✅ Old equipment properly unequipped
✅ Consumable items removed after use
✅ Equipment items removed when equipped
✅ Buff items increase max stats
✅ Console logging shows all actions
✅ Shop integration works
✅ Loot pickup integration works

## Next Steps

- Add proper inventory UI with item selection
- Add visual feedback when using items
- Add item icons in inventory display
- Add cooldown system for active items
- Add item descriptions in inventory UI
