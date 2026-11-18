.include "asm/include/interop_macros.inc"

.include "asm/include/scriptmacros.inc"
.include "asm/include/flags.inc"
.include "asm/include/soundeffects.inc"
.include "asm/include/vars.inc"

.include "asm/include/events.inc"
.include "asm/include/movements.inc"
.include "asm/include/std_scripts.inc"
.include "asm/include/trainers.inc"

#include "constants/item.h"

// Sprout Tower 3F (MAP_D15R0103, script file 18, text archive 056.txt).
// Byte-for-byte the vanilla script except that Elder Li gives the Flashlight (ITEM_FLASHLIGHT) instead of TM70 (Flash).

.data

scrdef scr_seq_D15R0103_000
scrdef scr_seq_D15R0103_001
scrdef scr_seq_D15R0103_002
scrdef_end

// map events (event_D15R0103.h)
.equ obj_D15R0103_chourou, 3 // Elder Li
.equ obj_D15R0103_gsrivel, 6 // rival

// on-transition: hide the rival once the rival scene has played
scr_seq_D15R0103_000:
    clearflag FLAG_UNK_1A3
    goto_if_set FLAG_UNK_078, _001F
    end

_001F:
    setflag FLAG_UNK_1A3
    end

// rival scene (coordinate trigger)
scr_seq_D15R0103_001:
    scrcmd_609
    lockall
    setvar VAR_UNK_40A4, 1
    apply_movement obj_player, _00B4
    wait_movement
    get_player_coords VAR_SPECIAL_x8004, VAR_SPECIAL_x8005
    scrcmd_102 VAR_SPECIAL_x8004, VAR_SPECIAL_x8005
    apply_movement 241, _00C0
    wait_movement
    apply_movement obj_D15R0103_chourou, _00D0
    wait_movement
    npc_msg 0
    closemsg
    wait 15, VAR_SPECIAL_RESULT
    callstd std_play_rival_outro_music
    buffer_rivals_name 0
    apply_movement obj_D15R0103_gsrivel, _00D8
    wait_movement
    npc_msg 1
    closemsg
    wait 15, VAR_SPECIAL_RESULT
    buffer_rivals_name 0
    npc_msg 2
    wait_ab_press
    closemsg
    play_se SEQ_SE_DP_KAIDAN2
    apply_movement obj_D15R0103_gsrivel, _00E0
    wait_movement
    hide_person obj_D15R0103_gsrivel
    setflag FLAG_UNK_078
    callstd std_fade_end_rival_outro_music
    apply_movement 241, _00C8
    wait_movement
    scrcmd_103
    releaseall
    end

.balign 4, 0
_00B4:
    step MOVEMENT_EMOTE_EXCLAMATION, 1
    step 63, 1 // Delay8
    step_end

.balign 4, 0
_00C0:
    step MOVEMENT_STEP_UP, 5
    step_end

.balign 4, 0
_00C8:
    step MOVEMENT_STEP_DOWN, 5
    step_end

.balign 4, 0
_00D0:
    step 33, 2 // walk on spot, facing south
    step_end

.balign 4, 0
_00D8:
    step MOVEMENT_STEP_DOWN, 1
    step_end

.balign 4, 0
_00E0: // rival spins, then leaves
    step MOVEMENT_FACE_LEFT, 4
    step MOVEMENT_FACE_UP, 4
    step MOVEMENT_FACE_RIGHT, 4
    step MOVEMENT_FACE_DOWN, 4
    step MOVEMENT_FACE_LEFT, 2
    step MOVEMENT_FACE_UP, 2
    step MOVEMENT_FACE_RIGHT, 2
    step MOVEMENT_FACE_DOWN, 2
    step MOVEMENT_FACE_LEFT, 2
    step MOVEMENT_FACE_UP, 2
    step MOVEMENT_FACE_RIGHT, 2
    step MOVEMENT_FACE_DOWN, 2
    step MOVEMENT_FACE_LEFT, 1
    step MOVEMENT_FACE_UP, 1
    step MOVEMENT_FACE_RIGHT, 1
    step MOVEMENT_FACE_DOWN, 1
    step MOVEMENT_FACE_LEFT, 1
    step MOVEMENT_FACE_UP, 1
    step MOVEMENT_FACE_RIGHT, 1
    step MOVEMENT_FACE_DOWN, 1
    step MOVEMENT_FACE_LEFT, 1
    step MOVEMENT_FACE_UP, 1
    step_end

// Elder Li
scr_seq_D15R0103_002:
    play_se SEQ_SE_DP_SELECT
    lockall
    faceplayer
    goto_if_set FLAG_UNK_076, _0197
    npc_msg 3
    closemsg
    trainer_battle TRAINER_ELDER_LI, 0, 0, 0
    check_battle_won VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq _01A2
    npc_msg 4
    giveitem_no_check ITEM_FLASHLIGHT, 1
    setflag FLAG_UNK_076
    setflag FLAG_HIDE_VIOLET_GYM_GYM_GUY_AFTER_SPROUT
    clearflag FLAG_HIDE_VIOLET_GYM_GYM_GUY_BEFORE_SPROUT
    npc_msg 5
    wait_button
    closemsg
    releaseall
    end

_0197:
    npc_msg 6
    wait_button
    closemsg
    releaseall
    end

_01A2:
    white_out
    releaseall
    end

.balign 4, 0
