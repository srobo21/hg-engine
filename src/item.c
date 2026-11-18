#include "../include/item.h"

#include "../include/config.h"
#include "../include/constants/file.h"
#include "../include/constants/item.h"
#include "../include/constants/moves.h"
#include "../include/bag.h"
#include "../include/message.h"
#include "../include/pokemon.h"
#include "../include/task.h"
#include "../include/script.h"
#include "../include/types.h"

#define GFX_ITEM_DUMMY_ID  ((MAX_TOTAL_ITEM_NUM) * 2 + 2)
#define GFX_ITEM_RETURN_ID ((MAX_TOTAL_ITEM_NUM + 1) * 2 + 4)
#define NEW_ITEM_GFX       (797)

u16 GetItemIndex(u16 item, u16 type);
void *GetItemNarcData(u16 item, u16 type, u32 heap_id);
u16 ItemToMachineMove(u16 itemId);
// void *LONG_CALL ItemDataTableLoad(int heapID);
void ItemMenuUseFunc_RevealGlass(struct ItemMenuUseData *data, const struct ItemCheckUseData *dat2);
BOOL ItemFieldUseFunc_RevealGlass(struct ItemFieldUseData *data);
void *_CreateRevealGlassWork(FieldSystem *fieldSystem);
void ItemMenuUseFunc_DNASplicers(struct ItemMenuUseData *data, const struct ItemCheckUseData *dat2);
BOOL ItemFieldUseFunc_DNASplicers(struct ItemFieldUseData *data);
void *_CreateDNASplicersWork(FieldSystem *fieldSystem);
void ItemMenuUseFunc_AbilityCapsule(struct ItemMenuUseData *data, const struct ItemCheckUseData *dat2);
void ItemMenuUseFunc_Mint(struct ItemMenuUseData *data, const struct ItemCheckUseData *dat2 UNUSED);
void ItemMenuUseFunc_Nectar(struct ItemMenuUseData *data, const struct ItemCheckUseData *dat2 UNUSED);
void ItemMenuUseFunc_RotomCatalog(struct ItemMenuUseData *data, const struct ItemCheckUseData *dat2 UNUSED);
void ItemMenuUseFunc_Flashlight(struct ItemMenuUseData *data, const struct ItemCheckUseData *dat2);
BOOL ItemFieldUseFunc_Flashlight(struct ItemFieldUseData *data);
u32 ItemCheckUseFunc_Flashlight(const struct ItemCheckUseData *data);
void ItemMenuUseFunc_InflatableRaft(struct ItemMenuUseData *data, const struct ItemCheckUseData *dat2);
BOOL ItemFieldUseFunc_InflatableRaft(struct ItemFieldUseData *data);
u32 ItemCheckUseFunc_InflatableRaft(const struct ItemCheckUseData *data);
void ItemMenuUseFunc_TransportSignal(struct ItemMenuUseData *data, const struct ItemCheckUseData *dat2);
BOOL ItemFieldUseFunc_TransportSignal(struct ItemFieldUseData *data);
u32 ItemCheckUseFunc_TransportSignal(const struct ItemCheckUseData *data);

const struct ItemUseFuncDat sNewItemFieldUseFuncs[] = {
    { ItemMenuUseFunc_RevealGlass, ItemFieldUseFunc_RevealGlass, NULL },
    { ItemMenuUseFunc_DNASplicers, ItemFieldUseFunc_DNASplicers, NULL },
    { ItemMenuUseFunc_AbilityCapsule, NULL, NULL },
    { ItemMenuUseFunc_Mint, NULL, NULL },
    { ItemMenuUseFunc_Nectar, NULL, NULL },
    { ItemMenuUseFunc_RotomCatalog, NULL, NULL },
    { ItemMenuUseFunc_Flashlight, ItemFieldUseFunc_Flashlight, ItemCheckUseFunc_Flashlight }, // NUM_VANILLA_FIELD_USE_FUNCS + 6
    { ItemMenuUseFunc_InflatableRaft, ItemFieldUseFunc_InflatableRaft, ItemCheckUseFunc_InflatableRaft }, // NUM_VANILLA_FIELD_USE_FUNCS + 7
    { ItemMenuUseFunc_TransportSignal, ItemFieldUseFunc_TransportSignal, ItemCheckUseFunc_TransportSignal }, // NUM_VANILLA_FIELD_USE_FUNCS + 8
};

extern const struct ItemUseFuncDat sItemFieldUseFuncs[NUM_VANILLA_FIELD_USE_FUNCS];

void *LONG_CALL GetItemFieldUseFunc(int funcType, u16 itemType)
{
    const struct ItemUseFuncDat *useFunc;
    if (itemType >= NUM_VANILLA_FIELD_USE_FUNCS) {
        u16 index = itemType - NUM_VANILLA_FIELD_USE_FUNCS;
        if (index >= NELEMS(sNewItemFieldUseFuncs)) {
            return NULL;
        }
        useFunc = &sNewItemFieldUseFuncs[index];
    } else {
        useFunc = &sItemFieldUseFuncs[itemType];
    }
    if (funcType == USE_ITEM_TASK_MENU) {
        return useFunc->menu;
    } else if (funcType == USE_ITEM_TASK_FIELD) {
        return useFunc->field;
    } else {
        return useFunc->check;
    }
}

u16 GetItemIndex(u16 item, u16 type)
{
    u16 ret = 0;
    switch (type) {
    case ITEM_GET_DATA:
        if (item == ITEM_DUMMY_ID || item == ITEM_RETURN_ID) {
            break;
        }
        ret = item;
        return ret;

    case ITEM_GET_ICON_CGX:
        if (item == ITEM_DUMMY_ID) {
            return GFX_ITEM_DUMMY_ID;
        }
        if (item == ITEM_RETURN_ID) {
            return GFX_ITEM_RETURN_ID;
        }
        ret = item * 2 + 2;
        return ret;

    case ITEM_GET_ICON_PAL:
        if (item == ITEM_DUMMY_ID) {
            return GFX_ITEM_DUMMY_ID + 1;
        }
        if (item == ITEM_RETURN_ID) {
            return GFX_ITEM_RETURN_ID + 1;
        }
        ret = item * 2 + 3;
        return ret;

    case ITEM_GET_AGB_NUM: // for pal park purposes
        if (item == ITEM_DUMMY_ID || item == ITEM_RETURN_ID) {
            break;
        }
        if (item > MAX_TOTAL_ITEM_NUM) {
            ret = 0;
        } else {
            ret = ItemDataIndex[item].agb_id;
        }
        return ret;
    }
    return 0;
}

void *GetItemNarcData(u16 item, u16 type, u32 heapID)
{
    int dataID, picID, palID;

    dataID = item;
    picID = item * 2 + 2;
    palID = item * 2 + 3;

    switch (type) {
    case ITEM_GET_DATA:
        return AllocAndReadWholeNarcMemberByIdPair(ARC_ITEM_DATA, dataID, heapID);
    case ITEM_GET_ICON_CGX:
        return AllocAndReadWholeNarcMemberByIdPair(ARC_ITEM_GFX_DATA, picID, heapID);
    case ITEM_GET_ICON_PAL:
        return AllocAndReadWholeNarcMemberByIdPair(ARC_ITEM_GFX_DATA, palID, heapID);
    }
    return NULL;
}

void LONG_CALL GetItemDescIntoString(String *dest, u16 itemId, u16 heapId)
{
    enum ItemGeneration gen = ITEM_GENERATION(itemId);
    u32 fileId = (gen == CUSTOM)
        ? MSG_DATA_ITEM_DESCRIPTION_CUSTOM
        : MSG_DATA_ITEM_FILE(MSG_DATA_ITEM_DESCRIPTION_GEN4, gen);
    MsgData *msgData = NewMsgDataFromNarc(MSGDATA_LOAD_LAZY, ARC_MSG_DATA, fileId, heapId);
    u32 offset = ITEM_MSG_OFFSET(itemId);
    ReadMsgDataIntoString(msgData, offset, dest);
    DestroyMsgData(msgData);
}

void *LONG_CALL ItemDataTableLoad(int heapID)
{
    int max = GetItemIndex(MAX_TOTAL_ITEM_NUM, ITEM_GET_DATA) + 1;

    return AllocAndReadFromNarcMemberByIdPair(ARC_ITEM_DATA, 0, heapID, 0, sizeof(ITEMDATA) * max); // 800757Ch
}

/**
 * @brief converts an item id to its corresponding TM/HM/TR index within sMachineMoves
 * @see   pret/pokeheartgold ItemToTMHMId
 */
u16 ItemToMachineMoveIndex(u16 itemId)
{
    if (itemId >= ITEM_TM001 && itemId <= ITEM_HM08) {
        return itemId - ITEM_TM001;
    }
    if (itemId == ITEM_HM07_ORAS) {
        return 100;
    }
    if (itemId == ITEM_TM00) {
        return 101;
    }
    if (itemId >= ITEM_TM093 && itemId <= ITEM_TM095) {
        return itemId - ITEM_TM093 + 102;
    }
    if (itemId >= ITEM_TM096 && itemId <= ITEM_TM100) {
        return itemId - ITEM_TM096 + 105;
    }
    if (itemId >= ITEM_TM100_SV && itemId <= ITEM_TM229) {
        return itemId - ITEM_TM100_SV + 110;
    }
    if (itemId >= ITEM_TR00 && itemId <= ITEM_TR99) {
        return itemId - ITEM_TR00 + 240;
    }

    return 0;
}

/**
 * @brief converts an item id to its corresponding TM/HM/TR move id
 * @see   pret/pokeheartgold TMHMGetMove
 */
u16 ItemToMachineMove(u16 itemId)
{
    if (itemId < ITEM_TM001) {
        return MOVE_NONE;
    }

    u16 index = ItemToMachineMoveIndex(itemId);
    if (index >= NUM_MACHINE_MOVES) {
        return MOVE_NONE;
    }
    u16 move = 0;
    ReadFromNarcMemberByIdPair(&move, ARC_CODE_ADDONS, CODE_ADDON_MACHINE_MOVES, sizeof(u16) * (index), sizeof(u16));
    return move;
}

BOOL MoveIsHM(u16 moveId)
{
    for (u8 i = 0; i < NUM_HMS; i++) {
        u16 moveTmp;
        u16 index = i + ITEM_HM01 - ITEM_TM001;
        ReadFromNarcMemberByIdPair(&moveTmp, ARC_CODE_ADDONS, CODE_ADDON_MACHINE_MOVES, sizeof(u16) * (index), sizeof(u16));
        if (moveTmp == moveId) {
#if defined(REUSABLE_TMS) && defined(DELETABLE_HMS)
            return FALSE;
#else
            return TRUE;
#endif
        }
    }
    return FALSE;
}

s32 LONG_CALL GetItemAttr_PreloadedItemData(ITEMDATA *itemData, u16 attrno)
{
    switch (attrno) {
    case ITEM_PARAM_PRICE:
        return (u32)((itemData)->price) | ((u32)((itemData)->price_high) << 16);
    case ITEM_PARAM_HOLD_EFFECT:
        return itemData->holdEffect;
    case ITEM_PARAM_HOLD_EFFECT_PARAM:
        return itemData->holdEffectParam;
    case ITEM_PARAM_PREVENT_TOSS:
        return itemData->prevent_toss;
    case ITEM_PARAM_SELECTABLE:
        return itemData->selectable;
    case ITEM_PARAM_FIELD_POCKET:
        return itemData->fieldPocket;
    case ITEM_PARAM_FIELD:
        return itemData->fieldUseFunc;
    case ITEM_PARAM_BATTLE:
        return itemData->battleUseFunc;
    case ITEM_PARAM_PLUCK_EFFECT:
        return itemData->pluckEffect;
    case ITEM_PARAM_FLING_EFFECT:
        return itemData->flingEffect;
    case ITEM_PARAM_FLING_POWER:
        return itemData->flingPower;
    case ITEM_PARAM_NATURAL_GIFT_POWER:
        return itemData->naturalGiftPower;
    case ITEM_PARAM_NATURAL_GIFT_TYPE:
        return itemData->naturalGiftType;
    case ITEM_PARAM_BATTLE_POCKET:
        return itemData->battlePocket;
    case ITEM_PARAM_PARTY:
        return itemData->partyUse;
    default:
        switch (itemData->partyUse) {
        case 0:
            return itemData->dummy;
        case 1:
            return GetItemAttrSub(&itemData->partyUseParam, attrno);
        }
    }

    return 0;
}

void ItemMenuUseFunc_RevealGlass(struct ItemMenuUseData *data, const struct ItemCheckUseData *dat2 UNUSED)
{
    FieldSystem *fieldSystem = data->taskManager->fieldSystem; // TaskManager_GetFieldSystem(data->taskManager);
    struct BagViewAppWork *env = data->taskManager->env; // TaskManager_GetEnvironment(data->taskManager);
    env->atexit_TaskEnv = sub_0203FAE8(fieldSystem, HEAPID_WORLD, ITEM_REVEAL_GLASS);
    sub_0203C8F0(env, 0x0203CA9C | 1);
}

BOOL ItemFieldUseFunc_RevealGlass(struct ItemFieldUseData *data)
{
    RegisteredItem_CreateGoToAppTask(data, (FieldApplicationWorkCtor)_CreateRevealGlassWork, FALSE);
    return TRUE;
}

void *_CreateRevealGlassWork(FieldSystem *fieldSystem)
{
    return sub_0203FAE8(fieldSystem, HEAPID_WORLD, ITEM_REVEAL_GLASS);
}

void ItemMenuUseFunc_DNASplicers(struct ItemMenuUseData *data, const struct ItemCheckUseData *dat2 UNUSED)
{
    FieldSystem *fieldSystem = data->taskManager->fieldSystem; // TaskManager_GetFieldSystem(data->taskManager);
    struct BagViewAppWork *env = data->taskManager->env; // TaskManager_GetEnvironment(data->taskManager);
    env->atexit_TaskEnv = sub_0203FAE8(fieldSystem, HEAPID_WORLD, ITEM_DNA_SPLICERS_FUSE); // TODO: handle correct item
    sub_0203C8F0(env, 0x0203CA9C | 1);
}

BOOL ItemFieldUseFunc_DNASplicers(struct ItemFieldUseData *data)
{
    RegisteredItem_CreateGoToAppTask(data, (FieldApplicationWorkCtor)_CreateDNASplicersWork, FALSE);
    return TRUE;
}

void *_CreateDNASplicersWork(FieldSystem *fieldSystem)
{
    return sub_0203FAE8(fieldSystem, HEAPID_WORLD, ITEM_DNA_SPLICERS_FUSE); // TODO: handle correct item
}

void ItemMenuUseFunc_AbilityCapsule(struct ItemMenuUseData *data, const struct ItemCheckUseData *dat2 UNUSED)
{
    FieldSystem *fieldSystem = data->taskManager->fieldSystem; // TaskManager_GetFieldSystem(data->taskManager);
    struct BagViewAppWork *env = data->taskManager->env; // TaskManager_GetEnvironment(data->taskManager);
    env->atexit_TaskEnv = sub_0203FAE8(fieldSystem, HEAPID_WORLD, ITEM_ABILITY_CAPSULE);
    sub_0203C8F0(env, 0x0203CA9C | 1);
}

void ItemMenuUseFunc_Mint(struct ItemMenuUseData *data, const struct ItemCheckUseData *dat2 UNUSED)
{
    FieldSystem *fieldSystem = data->taskManager->fieldSystem; // TaskManager_GetFieldSystem(data->taskManager);
    struct BagViewAppWork *env = data->taskManager->env; // TaskManager_GetEnvironment(data->taskManager);
    env->atexit_TaskEnv = sub_0203FAE8(fieldSystem, HEAPID_WORLD, data->itemId);
    sub_0203C8F0(env, 0x0203CA9C | 1);
}

void ItemMenuUseFunc_Nectar(struct ItemMenuUseData *data, const struct ItemCheckUseData *dat2 UNUSED)
{
    FieldSystem *fieldSystem = data->taskManager->fieldSystem; // TaskManager_GetFieldSystem(data->taskManager);
    struct BagViewAppWork *env = data->taskManager->env; // TaskManager_GetEnvironment(data->taskManager);
    env->atexit_TaskEnv = sub_0203FAE8(fieldSystem, HEAPID_WORLD, data->itemId);
    sub_0203C8F0(env, 0x0203CA9C | 1);
}

void ItemMenuUseFunc_RotomCatalog(struct ItemMenuUseData *data, const struct ItemCheckUseData *dat2 UNUSED)
{

    FieldSystem *fieldSystem = data->taskManager->fieldSystem; // TaskManager_GetFieldSystem(data->taskManager);
    struct BagViewAppWork *env = data->taskManager->env; // TaskManager_GetEnvironment(data->taskManager);
    env->atexit_TaskEnv = sub_0203FAE8(fieldSystem, HEAPID_WORLD, ITEM_ROTOM_CATALOG);
    sub_0203C8F0(env, 0x0203CA9C | 1);
}

/*
 * Flashlight: key item replacement for the Flash field move.
 *
 * Works anywhere vanilla Flash works: maps whose header weather is WEATHER_FLASH (dark caves) that
 * haven't been lit yet, and the Flash spot in the Ruins of Alph southeast chamber.
 * Used from the bag (menu) or from a Y-button registration (field).
 */
#define FLASHLIGHT_SCRIPT_ID          2074 // scr_seq_0003_074_flashlight in data/scr_seq/scr_seq_00003_commonscript.s
#define FLASHLIGHT_ALPH_SCRIPT_ID     2075 // scr_seq_0003_075_flashlight_alph
#define FLASHLIGHT_WEATHER_FLASH      11   // WEATHER_FLASH: dark map that hasn't been lit
#define FLASHLIGHT_FLAG_SYS_FLASH     2419 // FLAG_SYS_FLASH (0x973): set once the map has been lit
#define FLASHLIGHT_MAP_ALPH_SE_ROOM   314  // MAP_RUINS_OF_ALPH_SOUTHEAST_ENTRANCE_FIRST_ROOM
#define FLASHLIGHT_ITEM_USE_ERROR     0xFFFFFFFF // ITEMUSEERROR_OAKSWORDS ("not the time to use that")

BOOL LONG_CALL FieldSystem_FlagCheck(FieldSystem *fieldSystem, u16 flagId);

// mirrors CheckUseFlashInAlphChamber (pokeheartgold src/alph_checks.c)
static BOOL Flashlight_IsAlphFlashSpot(FieldSystem *fieldSystem)
{
    Location *position = fieldSystem->location;
    return position->mapId == FLASHLIGHT_MAP_ALPH_SE_ROOM && position->x == 6 && position->z == 3 && position->direction == 0; // DIR_NORTH
}

static u16 Flashlight_GetScript(FieldSystem *fieldSystem)
{
    return Flashlight_IsAlphFlashSpot(fieldSystem) ? FLASHLIGHT_ALPH_SCRIPT_ID : FLASHLIGHT_SCRIPT_ID;
}

u32 ItemCheckUseFunc_Flashlight(const struct ItemCheckUseData *data)
{
    FieldSystem *fieldSystem = data->fieldSystem;

    if (Flashlight_IsAlphFlashSpot(fieldSystem)) {
        return 0; // ITEMUSEERROR_OKAY
    }
    // same result as vanilla's LocalFieldData weather == WEATHER_FLASH check: entering a map with
    // FLAG_SYS_FLASH set turns WEATHER_FLASH into WEATHER_FLASH_2
    if (Fsys_GetWeather_HandleDiamondDust(fieldSystem, data->mapId) == FLASHLIGHT_WEATHER_FLASH
     && !FieldSystem_FlagCheck(fieldSystem, FLASHLIGHT_FLAG_SYS_FLASH)) {
        return 0; // ITEMUSEERROR_OKAY
    }
    return FLASHLIGHT_ITEM_USE_ERROR;
}

// used from the bag: close the menu, then queue the script (same path vanilla uses for items in the Alph chambers)
void ItemMenuUseFunc_Flashlight(struct ItemMenuUseData *data, const struct ItemCheckUseData *dat2)
{
    SetUpItemScript(data, dat2, Flashlight_GetScript(dat2->fieldSystem));
}

// used from the Y button: the check function has already passed
BOOL ItemFieldUseFunc_Flashlight(struct ItemFieldUseData *data)
{
    EventSet_Script(data->fieldSystem, Flashlight_GetScript(data->fieldSystem), NULL);
    return FALSE; // no app is launched, so the caller frees data
}

/*
 * Inflatable Raft: key item replacement for the Surf field move.
 *
 * - Bag or Y: facing surfable water (and not already surfing) runs common script 2076, which checks
 *   the Fog Badge and starts surfing.
 * - A on water: the field-move Surf script (file 146, scr_seq_0146_004) offers the raft first. That prompt
 *   only appears if GetIdxOfFirstPartyMonWithMove_Hook is installed (tools/find_surf_hook.py adds it).
 * - Surfing itself goes through the vanilla Surf task. CallFieldTask_Surf_Hook treats party slot
 *   RAFT_SURF_SLOT as "no Pokémon": it skips the cut-in and cry and keeps everything else vanilla.
 */
#define RAFT_SCRIPT_ID          2076 // scr_seq_0003_076_inflatable_raft
#define RAFT_SURF_SLOT          0xFF // party slot passed to the surf script command for the raft
#define RAFT_PLAYER_STATE_SURF  2    // PLAYER_STATE_SURFING
#define RAFT_MOVE_SURF          57   // MOVE_SURF

// vanilla overlay 1 functions (addresses from pret/pokeheartgold asm/overlay_01_*.s)
#define ov01_021F202C_StartSurfTask      ((void (*)(FieldSystem *, int, u32 *))(0x021F202C | 1))
#define ov01_021F2118_Task_Surf          ((TaskFunc)(0x021F2118 | 1))
#define ov01_021F20C0_CanSurfOnTile      ((BOOL (*)(void *, u32, u32))(0x021F20C0 | 1))
#define ov01_021F30D0_AllocSurfEnv       ((void *(*)(u32))(0x021F30D0 | 1))
#define ov01_021F3040_SetupCutInData     ((void (*)(FieldSystem *, void *, u32 *))(0x021F3040 | 1))
#define ov01_021F3100_GetPartyMon        ((void *(*)(FieldSystem *, int))(0x021F3100 | 1))
#define ov01_02205D68_FollowMonPrepare   ((BOOL (*)(FieldSystem *))(0x02205D68 | 1))
#define ov01_02206268_FollowMonActive    ((BOOL (*)(FieldSystem *))(0x02206268 | 1))
#define ov01_022062CC_FollowMonSlot      ((int (*)(FieldSystem *))(0x022062CC | 1))


// the vanilla surf task's environment (allocated by ov01_021F30D0, 0x2C bytes)
struct SurfTaskEnv {
    u32 state;              // 00
    u32 direction;          // 04
    u32 timer;              // 08
    u32 cutInData[4];       // 0C: show cut-in?, player gender, Pokemon *, cut-in task
    FieldSystem *fieldSystem; // 1C
    void *playerAvatar;     // 20
    void *playerMapObject;  // 24
    void *surfMount;        // 28
};

static inline void *Raft_GetAvatar(FieldSystem *fieldSystem)   { return *(void **)((u8 *)fieldSystem + 0x40); }
static inline s32 Raft_GetAvatarState(void *avatar)          { return *(s32 *)((u8 *)avatar + 0x18); }
static inline void *Raft_GetAvatarMapObject(void *avatar)    { return *(void **)((u8 *)avatar + 0x30); }

// wraps the vanilla surf task: state 2 is where the cut-in (or the follower's dive) starts, so go straight
// to what vanilla does after the cut-in (state 5 -> 6) instead
static BOOL Task_RaftSurf(TaskManager *taskManager)
{
    struct SurfTaskEnv *env = taskManager->env;
    if (env->state == 2) {
        ov01_02205D68_FollowMonPrepare(env->fieldSystem);
        env->state = 6;
        return FALSE;
    }
    return ov01_021F2118_Task_Surf(taskManager);
}

// hooked over vanilla CallFieldTask_Surf (overlay 1, 0x021F2068): called by the surf script command
void LONG_CALL CallFieldTask_Surf_Hook(TaskManager *taskManager, int playerDirection, int partySlot)
{
    FieldSystem *fieldSystem = taskManager->fieldSystem;
    void *avatar = Raft_GetAvatar(fieldSystem);

    if (partySlot == RAFT_SURF_SLOT) {
        struct SurfTaskEnv *env = ov01_021F30D0_AllocSurfEnv(sizeof(struct SurfTaskEnv)); // zeroed
        env->direction = playerDirection;
        env->fieldSystem = fieldSystem;
        env->playerAvatar = avatar;
        env->playerMapObject = Raft_GetAvatarMapObject(avatar);
        TaskManager_Call(*(TaskManager **)((u8 *)fieldSystem + 0x10), Task_RaftSurf, env);
        return;
    }

    // vanilla CallFieldTask_Surf
    u32 cutInData[4];
    void *mon = ov01_021F3100_GetPartyMon(fieldSystem, partySlot);
    ov01_021F3040_SetupCutInData(fieldSystem, mon, cutInData);
    u32 state = Raft_GetAvatarState(avatar);
    if (state - 1 <= 1) {
        cutInData[0] = TRUE;
    } else if (ov01_02206268_FollowMonActive(fieldSystem) && partySlot == ov01_022062CC_FollowMonSlot(fieldSystem)) {
        cutInData[0] = FALSE;
    }
    ov01_021F202C_StartSurfTask(fieldSystem, playerDirection, cutInData);
}

// hooked over vanilla GetIdxOfFirstPartyMonWithMove (arm9, address found by tools/find_surf_hook.py).
// Only the field's "can I surf here?" and "can I use Waterfall?" checks call it.
int LONG_CALL GetIdxOfFirstPartyMonWithMove_Hook(struct Party *party, u16 move)
{
    int count = party->count;
    for (int i = 0; i < count; i++) {
        struct PartyPokemon *mon = Party_GetMonByIndex(party, i);
        if (GetMonData(mon, MON_DATA_IS_EGG, NULL)) {
            continue;
        }
        if (GetMonData(mon, MON_DATA_MOVE1, NULL) == move || GetMonData(mon, MON_DATA_MOVE2, NULL) == move
         || GetMonData(mon, MON_DATA_MOVE3, NULL) == move || GetMonData(mon, MON_DATA_MOVE4, NULL) == move) {
            return i;
        }
    }
    if (move == RAFT_MOVE_SURF && Bag_HasItem(Sav2_Bag_get(gFieldSysPtr->savedata), ITEM_INFLATABLE_RAFT, 1, HEAPID_WORLD)) {
        return 6; // anything but 0xFF shows the surf prompt
    }
    return 0xFF;
}

u32 ItemCheckUseFunc_InflatableRaft(const struct ItemCheckUseData *data)
{
    void *avatar = Raft_GetAvatar(data->fieldSystem);
    if (Raft_GetAvatarState(avatar) == RAFT_PLAYER_STATE_SURF) {
        return FLASHLIGHT_ITEM_USE_ERROR; // already on the water
    }
    if (!ov01_021F20C0_CanSurfOnTile(avatar, data->standingTile, data->facingTile)) {
        return FLASHLIGHT_ITEM_USE_ERROR; // not facing surfable water
    }
    return 0; // the script checks the Fog Badge so it can say so
}

void ItemMenuUseFunc_InflatableRaft(struct ItemMenuUseData *data, const struct ItemCheckUseData *dat2)
{
    SetUpItemScript(data, dat2, RAFT_SCRIPT_ID);
}

BOOL ItemFieldUseFunc_InflatableRaft(struct ItemFieldUseData *data)
{
    EventSet_Script(data->fieldSystem, RAFT_SCRIPT_ID, NULL);
    return FALSE;
}

/*
 * Transport Signal: key item replacement for the Fly field move. It calls the Pokémon Transport,
 * which flies the player to any town they have visited.
 *
 * - Bag: opens the Fly map, exactly like choosing Fly from the party menu. Backing out of the map
 *   returns to the Bag.
 * - Y: fades out, opens the Fly map, then flies or fades back in.
 * - The flight is the vanilla Fly sequence (unk_02067A60.s sub_02067C30) run with no Pokémon: the
 *   follower is recalled, the player hops, and then the warp. The cry and the Pokémon cut-in are
 *   skipped, as with the other key items.
 * - Same checks as vanilla Fly (FieldMove_CheckFly): Storm Badge, map allows Fly, not in the Union Room
 *   or Colosseum, Safari Zone or Pal Park, no one with you, not in the Rocket disguise.
 */
#define SIGNAL_PARTY_SLOT        0xFF // no Pokémon; the fly task only recalls the follower for this
#define SIGNAL_BADGE_STORM       4
#define SIGNAL_FLAG_SYS_SAFARI   0x967
#define SIGNAL_FLAG_SYS_PAL_PARK 0x971
#define SIGNAL_MAP_LOAD_UNION    2
#define SIGNAL_MAP_LOAD_COLOSSEUM 3
#define SIGNAL_FLY_STATE_CUTIN   5    // sub_02067C30: wait for the hop, then start the cut-in
#define SIGNAL_FLY_STATE_WARP    6    // sub_02067C30: wait for the cut-in, then warp
#define SIGNAL_RGB_WHITE         0x7FFF
#define START_MENU_STATE_12      12   // load field, fade in, then jump to exitTaskFunc

// vanilla arm9 functions (addresses from pret/pokeheartgold's original asm)
#define PokegearTownMap_LaunchApp          ((void *(*)(FieldSystem *, int))(0x0203EAB0 | 1)) // was TownMap_New
#define MapHeader_IsFlyAllowed             ((BOOL (*)(u32))(0x0203B454 | 1))
#define FlyTask_New                        ((struct SignalFlyEnv *(*)(u32, FieldSystem *, void *, u16, u16, s16, s16))(0x02067BF8 | 1)) // sub_02067BF8
#define FlyTask_Run                        ((TaskFunc)(0x02067C30 | 1))                     // sub_02067C30
#define EventObjectMovementMan_IsFinish    ((BOOL (*)(void *))(0x02062260 | 1))
#define EventObjectMovementMan_Delete      ((void (*)(void *))(0x0206226C | 1))
#define Save_LocalFieldData_Get            ((void *(*)(void *))(0x0203B9C4 | 1))
#define LocalFieldData_GetSpecialSpawnWarpPtr ((void *(*)(void *))(0x0203B964 | 1))
#define FlyDest_GetSpawnId                 ((u16 (*)(u16))(0x0203BB50 | 1))                 // sub_0203BB50
#define GetFlyWarpData                     ((void (*)(u16, void *))(0x0203BA74 | 1))
#define GetSpecialSpawnWarpData            ((void (*)(u16, void *))(0x0203BAE8 | 1))
#define FieldTask_Warp                     ((void (*)(TaskManager *, u32, int, int, int, int))(0x02053908 | 1)) // sub_02053908
#define FieldSystem_CreateTask             ((TaskManager *(*)(FieldSystem *, TaskFunc, void *))(0x020504F0 | 1))
#define TaskManager_Jump                   ((void (*)(TaskManager *, TaskFunc, void *))(0x02050510 | 1))
#define IsPaletteFadeFinished              ((BOOL (*)(void))(0x0200FB5C | 1))
#define BeginNormalPaletteFade             ((void (*)(int, int, int, u16, int, int, u32))(0x0200FA24 | 1))
#define Signal_PauseAllMovement            ((void (*)(void *))(0x0205F574 | 1))
#define Signal_UnpauseAllMovement          ((void (*)(void *))(0x0205F5A4 | 1))
#define Task_ReturnToMenuFromAppItem       ((TaskFunc)(0x0203CA9C | 1))
// field overlay 1
#define ov01_FieldMap_FadeScreen           ((void (*)(int))(0x021E636C | 1))

BOOL LONG_CALL FieldSystem_ApplicationIsRunning(FieldSystem *fieldSystem);
void LONG_CALL FieldSystem_LoadFieldOverlay(FieldSystem *fieldSystem);
BOOL LONG_CALL sub_020505C8(FieldSystem *fieldSystem);

// PokegearArgs (pokeheartgold include/unk_02092BE8.h), only the fields the Fly map fills in
struct SignalMapArgs {
    u8 filler_00[0x14];
    BOOL setFlyDestination; // 14
    int mapCursorX;         // 18
    int mapCursorY;         // 1C
    u16 selectedFlyDest;    // 20
};

// environment of the vanilla fly task (0x1C bytes, made by sub_02067BF8)
struct SignalFlyEnv {
    FieldSystem *fieldSystem; // 00
    u16 state;                // 04
    u16 useFollower;          // 06
    u16 flyDest;              // 08
    s16 x;                    // 0A
    s16 y;                    // 0C
    u16 partySlot;            // 0E
    void *pokemon;            // 10
    void *cutInTask;          // 14
    void *movement;           // 18
};

// pokeheartgold Location
struct SignalLocation {
    int mapId;
    int warpId;
    int x;
    int z;
    int direction;
};

// vanilla StartMenuTaskData fields not named in struct BagViewAppWork
static inline TaskFunc *Signal_ExitTaskFunc(struct BagViewAppWork *env) { return (TaskFunc *)((u8 *)env + 0x354); }

static inline void *Signal_MapObjectManager(FieldSystem *fieldSystem) { return *(void **)((u8 *)fieldSystem + 0x3C); }
static inline int Signal_MapLoadType(FieldSystem *fieldSystem)      { return *(int *)((u8 *)fieldSystem + 0x70); }

static struct SignalFlyEnv *Signal_NewFlyEnv(FieldSystem *fieldSystem, struct SignalMapArgs *args)
{
    return FlyTask_New(HEAPID_WORLD, fieldSystem, NULL, SIGNAL_PARTY_SLOT, args->selectedFlyDest,
                       args->mapCursorX * 32 + 16, args->mapCursorY * 32 + 16);
}

// The vanilla fly task, minus the parts that need a Pokémon. With party slot 0xFF it already skips the
// cry and recalls the follower. This wrapper replaces the cut-in (states 5 and 6): after the hop it does
// the white fade-out the cut-in ends with, which the arrival animation expects, and then warps.
static BOOL Task_TransportSignalFly(TaskManager *taskManager)
{
    struct SignalFlyEnv *env = taskManager->env;

    switch (env->state) {
    case SIGNAL_FLY_STATE_CUTIN:
        if (EventObjectMovementMan_IsFinish(env->movement)) {
            EventObjectMovementMan_Delete(env->movement);
            // same fade as the end of the vanilla fly cut-in (ov02_02249420): both screens to white
            BeginNormalPaletteFade(0, 0, 0, SIGNAL_RGB_WHITE, 6, 1, 4);
            env->state = SIGNAL_FLY_STATE_WARP;
        }
        return FALSE;
    case SIGNAL_FLY_STATE_WARP:
        if (IsPaletteFadeFinished()) {
            // vanilla state 6, after the cut-in
            struct SignalLocation location;
            void *localFieldData = Save_LocalFieldData_Get(env->fieldSystem->savedata);
            u16 spawn = FlyDest_GetSpawnId(env->flyDest);
            GetFlyWarpData(spawn, &location);
            GetSpecialSpawnWarpData(spawn, LocalFieldData_GetSpecialSpawnWarpPtr(localFieldData));
            FieldTask_Warp(taskManager, location.mapId, -1, location.x, location.z, 1); // land facing south
            sys_FreeMemoryEz(env);
        }
        return FALSE;
    default:
        return FlyTask_Run(taskManager);
    }
}

u32 ItemCheckUseFunc_TransportSignal(const struct ItemCheckUseData *data)
{
    FieldSystem *fieldSystem = data->fieldSystem;
    int mapLoadType = Signal_MapLoadType(fieldSystem);

    if (mapLoadType == SIGNAL_MAP_LOAD_UNION || mapLoadType == SIGNAL_MAP_LOAD_COLOSSEUM) {
        return FLASHLIGHT_ITEM_USE_ERROR;
    }
    if (!PlayerProfile_TestBadgeFlag(Sav2_PlayerData_GetProfileAddr(fieldSystem->savedata), SIGNAL_BADGE_STORM)) {
        return FLASHLIGHT_ITEM_USE_ERROR;
    }
    if (!MapHeader_IsFlyAllowed(data->mapId)) {
        return FLASHLIGHT_ITEM_USE_ERROR;
    }
    if (data->haveFollower) {
        return 2; // ITEMUSEERROR_NOFOLLOWER: "can't use that while someone is with you"
    }
    if (data->haveRocketCostume) {
        return 3; // ITEMUSEERROR_NOTNOW
    }
    if (FieldSystem_FlagCheck(fieldSystem, SIGNAL_FLAG_SYS_SAFARI) || FieldSystem_FlagCheck(fieldSystem, SIGNAL_FLAG_SYS_PAL_PARK)) {
        return FLASHLIGHT_ITEM_USE_ERROR;
    }
    return 0;
}

// Bag: after the Fly map closes (StartMenu exit task)
static BOOL Task_TransportSignalMapClosed(TaskManager *taskManager)
{
    FieldSystem *fieldSystem = taskManager->fieldSystem;
    struct BagViewAppWork *startMenu = taskManager->env;
    struct SignalMapArgs *args = startMenu->atexit_TaskEnv;

    if (!args->setFlyDestination) {
        return Task_ReturnToMenuFromAppItem(taskManager); // frees the map args and reopens the Bag
    }
    struct SignalFlyEnv *flyEnv = Signal_NewFlyEnv(fieldSystem, args);
    Heap_FreeExplicit(HEAPID_WORLD, args);
    FieldSystem_LoadFieldOverlay(fieldSystem);
    *Signal_ExitTaskFunc(startMenu) = Task_TransportSignalFly;
    startMenu->atexit_TaskEnv = flyEnv;
    startMenu->state = START_MENU_STATE_12;
    return FALSE;
}

void ItemMenuUseFunc_TransportSignal(struct ItemMenuUseData *data, const struct ItemCheckUseData *dat2 UNUSED)
{
    FieldSystem *fieldSystem = data->taskManager->fieldSystem;
    struct BagViewAppWork *env = data->taskManager->env;
    env->atexit_TaskEnv = PokegearTownMap_LaunchApp(fieldSystem, 0); // 0 = Fly map
    sub_0203C8F0(env, (u32)Task_TransportSignalMapClosed);
}

// Y: modelled on vanilla Task_RegisteredItem_GoToApp. data->work holds the map args, then the fly env.
static BOOL Task_TransportSignalY(TaskManager *taskManager)
{
    FieldSystem *fieldSystem = taskManager->fieldSystem;
    struct ItemFieldUseData *data = taskManager->env;

    switch (data->state) {
    case 0:
        Signal_PauseAllMovement(Signal_MapObjectManager(fieldSystem));
        ov01_FieldMap_FadeScreen(0); // FADE_TYPE_BRIGHTNESS_OUT
        data->state = 1;
        break;
    case 1:
        if (IsPaletteFadeFinished()) {
            data->work = PokegearTownMap_LaunchApp(fieldSystem, 0);
            data->state = 2;
        }
        break;
    case 2:
        if (FieldSystem_ApplicationIsRunning(fieldSystem)) {
            break;
        }
        {
            struct SignalMapArgs *args = data->work;
            data->no_app = args->setFlyDestination ? TRUE : FALSE; // reused as "flying"
            data->work = data->no_app ? Signal_NewFlyEnv(fieldSystem, args) : NULL;
            sys_FreeMemoryEz(args);
        }
        FieldSystem_LoadFieldOverlay(fieldSystem);
        data->state = 3;
        break;
    case 3:
        if (sub_020505C8(fieldSystem)) {
            Signal_PauseAllMovement(Signal_MapObjectManager(fieldSystem));
            ov01_FieldMap_FadeScreen(1); // FADE_TYPE_BRIGHTNESS_IN
            data->state = 4;
        }
        break;
    case 4:
        if (IsPaletteFadeFinished()) {
            if (data->no_app) {
                void *flyEnv = data->work;
                sys_FreeMemoryEz(data);
                TaskManager_Jump(taskManager, Task_TransportSignalFly, flyEnv);
                return FALSE;
            }
            Signal_UnpauseAllMovement(Signal_MapObjectManager(fieldSystem));
            sys_FreeMemoryEz(data);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

BOOL ItemFieldUseFunc_TransportSignal(struct ItemFieldUseData *data)
{
    data->state = 0;
    FieldSystem_CreateTask(data->fieldSystem, Task_TransportSignalY, data);
    return TRUE; // the task owns data now
}
