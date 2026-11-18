.include "asm/include/interop_macros.inc"

.include "asm/include/scriptmacros.inc"
.include "asm/include/flags.inc"
.include "asm/include/soundeffects.inc"
.include "asm/include/vars.inc"

.include "asm/include/events.inc"
.include "asm/include/game_stats.inc"
.include "asm/include/maps.inc"
.include "asm/include/map_sections.inc"
.include "asm/include/movements.inc"
.include "asm/include/rankings.inc"
.include "asm/include/spawns.inc"
.include "asm/include/std_scripts.inc"
.include "asm/include/trainers.inc"

#include "constants/item.h"
#include "constants/moves.h"
#include "constants/species.h"


// Goldenrod Game Corner (slot machine version) - MAP_GOLDENROD_GAME_CORNER_JP (map 183, T25R1101)
// script archive: 906   text archive to grab from: 599.txt   events: zone_event 176
//
// The script order below must not change: the map's events point at these slots by number.
//   000  coin seller (front counter)          005  "slot tips" man
//   001  TM / held item prize clerk           006-009  flavor NPCs
//   002  Pokemon prize clerk                  010-031  the 22 slot machines (bg events)
//   003  gentleman (hidden Sun 7-9am)         032  on-transition (hides the gentleman)
//   004  man by the left row of machines
//
// NOTE: retail US/EU HeartGold has no slot machine minigame. casino_game (scrcmd 780)
// launches Voltorb Flip; the machine id only changes that machine's "luck".

// coins needed to sit down at a machine
.equ SLOT_MIN_COINS, 3

// VAR_TEMP_x4003 = prize cost in coins (used by the shared buy routines)

.data

scrdef scr_seq_0906_000
scrdef scr_seq_0906_001
scrdef scr_seq_0906_002
scrdef scr_seq_0906_003
scrdef scr_seq_0906_004
scrdef scr_seq_0906_005
scrdef scr_seq_0906_006
scrdef scr_seq_0906_007
scrdef scr_seq_0906_008
scrdef scr_seq_0906_009
scrdef scr_seq_0906_010
scrdef scr_seq_0906_011
scrdef scr_seq_0906_012
scrdef scr_seq_0906_013
scrdef scr_seq_0906_014
scrdef scr_seq_0906_015
scrdef scr_seq_0906_016
scrdef scr_seq_0906_017
scrdef scr_seq_0906_018
scrdef scr_seq_0906_019
scrdef scr_seq_0906_020
scrdef scr_seq_0906_021
scrdef scr_seq_0906_022
scrdef scr_seq_0906_023
scrdef scr_seq_0906_024
scrdef scr_seq_0906_025
scrdef scr_seq_0906_026
scrdef scr_seq_0906_027
scrdef scr_seq_0906_028
scrdef scr_seq_0906_029
scrdef scr_seq_0906_030
scrdef scr_seq_0906_031
scrdef scr_seq_0906_032
scrdef_end


// ============================================================================
// 032 - on transition: hide the gentleman on Sunday between 7:00 and 9:59
// ============================================================================
scr_seq_0906_032:
    get_weekday VAR_TEMP_x4000
    compare VAR_TEMP_x4000, 0
    goto_if_ne _ShowGentleman
    scrcmd_522 VAR_TEMP_x4000           // current hour
    compare VAR_TEMP_x4000, 7
    goto_if_lt _ShowGentleman
    compare VAR_TEMP_x4000, 9
    goto_if_gt _ShowGentleman
    setflag FLAG_UNK_2DA
    end

_ShowGentleman:
    clearflag FLAG_UNK_2DA
    end


// ============================================================================
// 010-031 - slot machines
// ============================================================================
.macro slot_machine,id,cancel
    play_se SEQ_SE_DP_SELECT
    lockall
    call _SlotCanPlay
    compare VAR_TEMP_x4001, 1
    goto_if_eq \cancel
    fade_screen 6, 1, 0, RGB_BLACK
    wait_fade
    casino_game \id, 0                  // 0 = Goldenrod, 1 = Celadon
    restore_overworld
    fade_screen 6, 1, 1, RGB_BLACK
    wait_fade
\cancel:
    releaseall
    end
.endm

scr_seq_0906_010: slot_machine 0, _Slot010Done
scr_seq_0906_011: slot_machine 1, _Slot011Done
scr_seq_0906_012: slot_machine 2, _Slot012Done
scr_seq_0906_013: slot_machine 3, _Slot013Done
scr_seq_0906_014: slot_machine 4, _Slot014Done
scr_seq_0906_015: slot_machine 5, _Slot015Done
scr_seq_0906_016: slot_machine 6, _Slot016Done
scr_seq_0906_017: slot_machine 7, _Slot017Done
scr_seq_0906_018: slot_machine 8, _Slot018Done
scr_seq_0906_019: slot_machine 9, _Slot019Done
scr_seq_0906_020: slot_machine 10, _Slot020Done
scr_seq_0906_021: slot_machine 11, _Slot021Done
scr_seq_0906_022: slot_machine 21, _Slot022Done
scr_seq_0906_023: slot_machine 12, _Slot023Done
scr_seq_0906_024: slot_machine 13, _Slot024Done
scr_seq_0906_025: slot_machine 14, _Slot025Done
scr_seq_0906_026: slot_machine 15, _Slot026Done
scr_seq_0906_027: slot_machine 16, _Slot027Done
scr_seq_0906_028: slot_machine 17, _Slot028Done
scr_seq_0906_029: slot_machine 18, _Slot029Done
scr_seq_0906_030: slot_machine 19, _Slot030Done
scr_seq_0906_031: slot_machine 20, _Slot031Done

// sets VAR_TEMP_x4001 to 0 if the player can play, 1 if not
_SlotCanPlay:
    hasitem ITEM_COIN_CASE, 1, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq _SlotNoCoinCase
    script_overlay_cmd 3, 0             // coin commands live in this overlay
    check_coins_immediate VAR_SPECIAL_RESULT, SLOT_MIN_COINS
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq _SlotNotEnoughCoins
    script_overlay_cmd 3, 1
    setvar VAR_TEMP_x4001, 0
    return

_SlotNoCoinCase:
    npc_msg 25
    wait_button
    closemsg
    setvar VAR_TEMP_x4001, 1
    return

_SlotNotEnoughCoins:
    npc_msg 26
    wait_button
    closemsg
    script_overlay_cmd 3, 1
    setvar VAR_TEMP_x4001, 1
    return


// ============================================================================
// 000 - coin seller
// ============================================================================
scr_seq_0906_000:
    play_se SEQ_SE_DP_SELECT
    lockall
    faceplayer
    npc_msg 0
    hasitem ITEM_COIN_CASE, 1, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 1
    goto_if_eq _CoinSellerOpen
    npc_msg 1
    wait_button
    closemsg
    releaseall
    end

_CoinSellerOpen:
    script_overlay_cmd 3, 0
    npc_msg 2
    touchscreen_menu_hide
    scrcmd_116 0, 20, 2                 // coin box
    show_money_box 20, 7
_CoinSellerMenu:
    // std text 127-129: "50 Coins $1,000" / "500 Coins $10,000" / "NO THANKS"
    menu_init_std_gmm 1, 1, 0, 1, VAR_SPECIAL_RESULT
    menu_item_add 127, 255, 0
    menu_item_add 128, 255, 1
    menu_item_add 129, 255, 2
    menu_exec
    switch VAR_SPECIAL_RESULT
    case 0, _CoinSellerBuy50
    case 1, _CoinSellerBuy500
    npc_msg 6
    goto _CoinSellerExit

_CoinSellerBuy50:
    hasenoughmoneyimmediate VAR_SPECIAL_RESULT, 1000
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq _CoinSellerNoMoney
    checkgivecoins VAR_SPECIAL_RESULT, 50
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq _CoinSellerCaseFull
    submoneyimmediate 1000
    give_coins 50
    goto _CoinSellerSold

_CoinSellerBuy500:
    hasenoughmoneyimmediate VAR_SPECIAL_RESULT, 10000
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq _CoinSellerNoMoney
    checkgivecoins VAR_SPECIAL_RESULT, 500
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq _CoinSellerCaseFull
    submoneyimmediate 10000
    give_coins 500
_CoinSellerSold:
    scrcmd_118 0                        // refresh coin box
    update_money_box
    play_se SEQ_SE_DP_REGI
    wait_se SEQ_SE_DP_REGI
    npc_msg 3
    goto _CoinSellerMenu

_CoinSellerCaseFull:
    npc_msg 5
    goto _CoinSellerExit

_CoinSellerNoMoney:
    npc_msg 4
_CoinSellerExit:
    wait_button
    closemsg
    touchscreen_menu_show
    scrcmd_117                          // hide coin box
    hide_money_box
    script_overlay_cmd 3, 1
    releaseall
    end


// ============================================================================
// 001 - TM / held item prize clerk
// ============================================================================
scr_seq_0906_001:
    play_se SEQ_SE_DP_SELECT
    lockall
    faceplayer
    script_overlay_cmd 3, 0
    npc_msg 7
    touchscreen_menu_hide
    scrcmd_116 0, 20, 2
_PrizeTopMenu:
    menu_init 1, 1, 0, 1, VAR_TEMP_x4000
    menu_item_add 37, 255, 0            // TMs
    menu_item_add 38, 255, 1            // Held items
    menu_item_add 36, 255, 2            // Cancel
    menu_exec
    switch VAR_TEMP_x4000
    case 0, _PrizeTMMenu
    case 1, _PrizeHeldMenu
    goto _PrizeGoodbye

_PrizeTMMenu:
    setvar VAR_TEMP_x4004, 0            // remember which list to return to
    menu_init 1, 1, 0, 1, VAR_TEMP_x4000
    menu_item_add 39, 255, 0
    menu_item_add 40, 255, 1
    menu_item_add 41, 255, 2
    menu_item_add 42, 255, 3
    menu_item_add 43, 255, 4
    menu_item_add 44, 255, 5
    menu_item_add 36, 255, 6
    menu_exec
    switch VAR_TEMP_x4000
    case 0, _PrizeTM90
    case 1, _PrizeTM75
    case 2, _PrizeTM44
    case 3, _PrizeTM35
    case 4, _PrizeTM13
    case 5, _PrizeTM24
    goto _PrizeTopMenu

_PrizeTM90:
    setvar VAR_SPECIAL_x8004, ITEM_TM090    // Substitute
    setvar VAR_TEMP_x4003, 2000
    goto _PrizeBuyItem
_PrizeTM75:
    setvar VAR_SPECIAL_x8004, ITEM_TM075    // Swords Dance
    setvar VAR_TEMP_x4003, 4000
    goto _PrizeBuyItem
_PrizeTM44:
    setvar VAR_SPECIAL_x8004, ITEM_TM044    // Rest
    setvar VAR_TEMP_x4003, 6000
    goto _PrizeBuyItem
_PrizeTM35:
    setvar VAR_SPECIAL_x8004, ITEM_TM035    // Flamethrower
    setvar VAR_TEMP_x4003, 10000
    goto _PrizeBuyItem
_PrizeTM13:
    setvar VAR_SPECIAL_x8004, ITEM_TM013    // Ice Beam
    setvar VAR_TEMP_x4003, 10000
    goto _PrizeBuyItem
_PrizeTM24:
    setvar VAR_SPECIAL_x8004, ITEM_TM024    // Thunderbolt
    setvar VAR_TEMP_x4003, 10000
    goto _PrizeBuyItem

_PrizeHeldMenu:
    setvar VAR_TEMP_x4004, 1
    menu_init 1, 1, 0, 1, VAR_TEMP_x4000
    menu_item_add 45, 255, 0
    menu_item_add 46, 255, 1
    menu_item_add 47, 255, 2
    menu_item_add 48, 255, 3
    menu_item_add 36, 255, 4
    menu_exec
    switch VAR_TEMP_x4000
    case 0, _PrizeSilkScarf
    case 1, _PrizeWideLens
    case 2, _PrizeZoomLens
    case 3, _PrizeMetronome
    goto _PrizeTopMenu

_PrizeSilkScarf:
    setvar VAR_SPECIAL_x8004, ITEM_SILK_SCARF
    setvar VAR_TEMP_x4003, 1000
    goto _PrizeBuyItem
_PrizeWideLens:
    setvar VAR_SPECIAL_x8004, ITEM_WIDE_LENS
    setvar VAR_TEMP_x4003, 1000
    goto _PrizeBuyItem
_PrizeZoomLens:
    setvar VAR_SPECIAL_x8004, ITEM_ZOOM_LENS
    setvar VAR_TEMP_x4003, 1000
    goto _PrizeBuyItem
_PrizeMetronome:
    setvar VAR_SPECIAL_x8004, ITEM_METRONOME
    setvar VAR_TEMP_x4003, 1000
    goto _PrizeBuyItem

// in: VAR_SPECIAL_x8004 = item, VAR_TEMP_x4003 = cost, VAR_TEMP_x4004 = list to return to
_PrizeBuyItem:
    setvar VAR_SPECIAL_x8005, 1
    hasspaceforitem VAR_SPECIAL_x8004, VAR_SPECIAL_x8005, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq _PrizeBagFull
    buffer_item_name 0, VAR_SPECIAL_x8004
    npc_msg 10                          // "<item>, right?"
    getmenuchoice VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_ne _PrizeBackToList
    get_coin_amount VAR_SPECIAL_x8006
    compare VAR_SPECIAL_x8006, VAR_TEMP_x4003
    goto_if_lt _PrizeNotEnoughCoins
    npc_msg 11
    play_se SEQ_SE_DP_REGI
    take_coins VAR_TEMP_x4003
    scrcmd_118 0
    buffer_item_name 0, VAR_SPECIAL_x8004
    getitempocket VAR_SPECIAL_x8004, VAR_SPECIAL_RESULT
    buffer_pocket_name 1, VAR_SPECIAL_RESULT
    npc_msg 17                          // "You put away the <item> in the <pocket>."
    giveitem VAR_SPECIAL_x8004, 1, VAR_SPECIAL_RESULT
    wait_button
    goto _PrizeBackToList

_PrizeBagFull:
    npc_msg 12
    wait_button
    goto _PrizeBackToList

_PrizeNotEnoughCoins:
    npc_msg 9
    wait_button
_PrizeBackToList:
    closemsg
    compare VAR_TEMP_x4004, 0
    goto_if_eq _PrizeTMMenu
    goto _PrizeHeldMenu

_PrizeGoodbye:
    npc_msg 8
    wait_button
    closemsg
_PrizeExit:
    scrcmd_117
    touchscreen_menu_show
    script_overlay_cmd 3, 1
    releaseall
    end


// ============================================================================
// 002 - Pokemon prize clerk
// ============================================================================
scr_seq_0906_002:
    play_se SEQ_SE_DP_SELECT
    lockall
    faceplayer
    script_overlay_cmd 3, 0
    npc_msg 16
    touchscreen_menu_hide
    scrcmd_116 0, 20, 2
_MonPrizeMenu:
    get_game_version VAR_TEMP_x4000
    compare VAR_TEMP_x4000, 7           // 7 = HeartGold
    goto_if_ne _MonPrizeMenuSS
    menu_init 1, 1, 0, 1, VAR_TEMP_x4000
    menu_item_add 49, 255, 0            // Abra
    menu_item_add 50, 255, 1            // Ekans
    menu_item_add 51, 255, 2            // Dratini
    menu_item_add 36, 255, 3
    menu_exec
    goto _MonPrizeChoice

_MonPrizeMenuSS:
    menu_init 1, 1, 0, 1, VAR_TEMP_x4000
    menu_item_add 49, 255, 0            // Abra
    menu_item_add 52, 255, 1            // Sandshrew
    menu_item_add 51, 255, 2            // Dratini
    menu_item_add 36, 255, 3
    menu_exec
_MonPrizeChoice:
    switch VAR_TEMP_x4000
    case 0, _MonPrizeAbra
    case 1, _MonPrizeSlot2
    case 2, _MonPrizeDratini
    goto _PrizeGoodbye

_MonPrizeAbra:
    setvar VAR_TEMP_x4002, SPECIES_ABRA
    setvar VAR_TEMP_x4003, 200
    goto _MonPrizeBuy
_MonPrizeSlot2:
    setvar VAR_TEMP_x4002, SPECIES_EKANS
    setvar VAR_TEMP_x4003, 700
    get_game_version VAR_TEMP_x4000
    compare VAR_TEMP_x4000, 7
    goto_if_eq _MonPrizeBuy
    setvar VAR_TEMP_x4002, SPECIES_SANDSHREW
    goto _MonPrizeBuy
_MonPrizeDratini:
    setvar VAR_TEMP_x4002, SPECIES_DRATINI
    setvar VAR_TEMP_x4003, 2100

// in: VAR_TEMP_x4002 = species, VAR_TEMP_x4003 = cost
_MonPrizeBuy:
    get_party_count VAR_SPECIAL_x8005
    compare VAR_SPECIAL_x8005, 6
    goto_if_ne _MonPrizeConfirm
    npc_msg 13                          // party full
    wait_button
    closemsg
    goto _PrizeExit

_MonPrizeConfirm:
    buffer_species_name 1, VAR_TEMP_x4002, 0, 0
    npc_msg 14                          // "Would you like <species>?"
    getmenuchoice VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_ne _MonPrizeBackToList
    get_coin_amount VAR_SPECIAL_x8006
    compare VAR_SPECIAL_x8006, VAR_TEMP_x4003
    goto_if_lt _MonPrizeNotEnoughCoins
    npc_msg 11
    play_se SEQ_SE_DP_REGI
    give_mon VAR_TEMP_x4002, 15, 0, 0, 0, VAR_SPECIAL_RESULT
    take_coins VAR_TEMP_x4003
    scrcmd_118 0
    buffer_players_name 0
    buffer_species_name 1, VAR_TEMP_x4002, 0, 0
    npc_msg 15                          // "<player> received <species>!"
    wait_button
    goto _MonPrizeBackToList

_MonPrizeNotEnoughCoins:
    npc_msg 9
    wait_button
_MonPrizeBackToList:
    closemsg
    goto _MonPrizeMenu


// ============================================================================
// 003-009 - people
// ============================================================================
scr_seq_0906_003:
    simple_npc_msg 18
    end

scr_seq_0906_004:
    simple_npc_msg 19
    end

// slot tips: keeps offering another tip until the player says no
scr_seq_0906_005:
    play_se SEQ_SE_DP_SELECT
    lockall
    faceplayer
    npc_msg 27                          // "Want some tips?"
    call _TipsYesNo
    compare VAR_SPECIAL_RESULT, 1
    goto_if_eq _TipsNo
    npc_msg 31
    wait_button
    npc_msg 28                          // "Want to hear more?"
    call _TipsYesNo
    compare VAR_SPECIAL_RESULT, 1
    goto_if_eq _TipsNo
    npc_msg 32
    wait_button
    npc_msg 28
    call _TipsYesNo
    compare VAR_SPECIAL_RESULT, 1
    goto_if_eq _TipsNo
    npc_msg 33
    wait_button
    npc_msg 28
    call _TipsYesNo
    compare VAR_SPECIAL_RESULT, 1
    goto_if_eq _TipsNo
    npc_msg 34
    wait_button
    npc_msg 28
    call _TipsYesNo
    compare VAR_SPECIAL_RESULT, 1
    goto_if_eq _TipsNo
    npc_msg 35
    wait_button
    npc_msg 30                          // "That's all I know"
    wait_button
    closemsg
    releaseall
    end

// VAR_SPECIAL_RESULT: 0 = yes, 1 = no
_TipsYesNo:
    touchscreen_menu_hide
    getmenuchoice VAR_SPECIAL_RESULT
    touchscreen_menu_show
    return

_TipsNo:
    npc_msg 29
    wait_button
    closemsg
    releaseall
    end

scr_seq_0906_006:
    simple_npc_msg 21
    end

scr_seq_0906_007:
    simple_npc_msg 22
    end

scr_seq_0906_008:
    simple_npc_msg 23
    end

scr_seq_0906_009:
    simple_npc_msg 24
    end

.balign 4, 0
