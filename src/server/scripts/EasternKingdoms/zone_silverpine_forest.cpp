/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "CellImpl.h"
#include "CombatAI.h"
#include "CreatureAIImpl.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "GridNotifiers.h"
#include "Group.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Object.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "SharedDefines.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "SpellScript.h"
#include "SpellMgr.h"
#include "TemporarySummon.h"
#include "Unit.h"
#include "Vehicle.h"

Position const OrgrimmarPortalPos[3] =
{
    { 1358.62f, 1054.72f, 53.1200f, 0.0f },
    { 1393.27f, 1021.20f, 53.2225f, 0.0f },
    { 1404.71f, 1063.73f, 60.5617f, 0.0f }
};

Position const HellscreamElitePos[16] =
{
    { 1387.90f, 1029.71f, 53.21853f, 2.827433f },
    { 1389.79f, 1024.51f, 53.20833f, 2.827433f },
    { 1388.05f, 1026.91f, 53.20833f, 2.827433f },
    { 1388.16f, 1020.88f, 53.25523f, 2.827433f },
    { 1385.79f, 1025.99f, 53.22593f, 2.827433f },
    { 1386.69f, 1023.26f, 53.24393f, 2.827433f },
    { 1384.33f, 1022.04f, 53.28123f, 2.827433f },
    { 1391.10f, 1027.73f, 53.20483f, 2.827433f },

    { 1359.10f, 1046.55f, 52.97053f, 5.253441f },
    { 1360.89f, 1051.81f, 53.19793f, 5.253441f },
    { 1360.75f, 1048.84f, 53.12893f, 5.253441f },
    { 1364.43f, 1053.62f, 53.29343f, 5.253441f },
    { 1363.08f, 1048.15f, 53.22223f, 5.253441f },
    { 1364.08f, 1050.84f, 53.29163f, 5.253441f },
    { 1366.69f, 1050.31f, 53.34203f, 5.253441f },
    { 1357.85f, 1050.12f, 52.99823f, 5.253441f }
};

Position const GarroshPos = { 1402.45f, 1061.62f, 60.56173f, 3.926991f };

Position const GarroshJumpPos = { 1378.65f, 1044.23f, 53.8389f, 5.51524f };

Position const CromushPos = { 1404.71f, 1063.73f, 60.5617f, 2.827433f };

Position const AgathaPreRisePos = { 1364.02f, 1028.54f, 66.99143f };

Position const AgathaRisePos = { 1368.65f, 1032.19f, 63.3033f };

Position const AgathaPreResetPos = { 1364.02f, 1028.54f, 55.9914f };

Position const AgathaResetPos = { 1364.02f, 1028.54f, 58.1319f };

enum QuestTheWarchiefCometh
{
    QUEST_THE_WARCHIEF_COMETH               = 26965,

    NPC_LADY_SYLVANAS_WINDRUNNER_COMETH     = 44365,
    NPC_AGATHA_COMETH                       = 44608,
    NPC_GRAND_EXECUTOR_MORTUUS              = 44615,
    NPC_MALE_FALLEN_HUMAN                   = 44592,
    NPC_FEMALE_FALLEN_HUMAN                 = 44593,
    NPC_PORTAL_FROM_ORGRIMMAR               = 44630,
    NPC_GARROSH_HELLSCREAM                  = 44629,
    NPC_HIGH_WARLORD_CROMUSH_COMETH         = 44640,
    NPC_HELLSCREAM_ELITE_COMETH             = 44636,
    NPC_QUEST_MONSTER_CREDIT                = 44629,
    NPC_FORSAKEN_WARHORSE_UNPHASED          = 73595,

    SPELL_RAISE_FORSAKEN_COMETH             = 83173,
    SPELL_AIR_REVENANT_ENTRANCE             = 55761,
    SPELL_SIMPLE_TELEPORT                   = 12980,
    SPELL_WELCOME_TO_SILVERPINE_CREDIT      = 83384,

    EVENT_START_SCENE_COMETH                = 1,
    EVENT_SUMMON_PORTAL_COMETH              = 2,
    EVENT_SUMMON_GARROSH_COMETH             = 3,
    EVENT_AGATHA_RAISE_FORSAKEN             = 4, // Note: 4-8 are used
    EVENT_SCENE_TALK_COMETH                 = 9, // Note: 9-36 are used

    ACTION_START_SCENE_COMETH               = 1,

    TALK_SYLVANAS_COMETH_0                  = 0,
    TALK_SYLVANAS_COMETH_1                  = 1,
    TALK_SYLVANAS_COMETH_2                  = 2,
    TALK_SYLVANAS_COMETH_3                  = 3,
    TALK_SYLVANAS_COMETH_4                  = 4,
    TALK_SYLVANAS_COMETH_5                  = 5,
    TALK_SYLVANAS_COMETH_6                  = 6,
    TALK_SYLVANAS_COMETH_7                  = 7,
    TALK_SYLVANAS_COMETH_8                  = 8,
    TALK_SYLVANAS_COMETH_9                  = 9,
    TALK_SYLVANAS_COMETH_10                 = 10,
    TALK_GARROSH_COMETH_0                   = 0,
    TALK_GARROSH_COMETH_1                   = 1,
    TALK_GARROSH_COMETH_2                   = 2,
    TALK_GARROSH_COMETH_3                   = 3,
    TALK_GARROSH_COMETH_4                   = 4,
    TALK_GARROSH_COMETH_5                   = 5,
    TALK_GARROSH_COMETH_6                   = 6,
    TALK_GARROSH_COMETH_7                   = 7,
    TALK_GARROSH_COMETH_8                   = 8,
    TALK_GARROSH_COMETH_9                   = 9,
    TALK_GARROSH_COMETH_10                  = 10,
    TALK_CROMUSH_COMETH_0                   = 0,
    TALK_CROMUSH_COMETH_1                   = 1,

    PATH_CROMUSH                            = 446402,
    PATH_GARROSH                            = 446290,

    POINT_AGATHA_PRE_RISE                   = 1,
    POINT_AGATHA_RISE                       = 2,
    POINT_AGATHA_PRE_RESET                  = 3,
    POINT_AGATHA_RESET                      = 4,

    ANIMKIT_GENERAL_1                       = 609,
    ANIMKIT_SYLV_1                          = 595,
    ANIMKIT_SYLV_2                          = 606,
    ANIMKIT_GARROSH_1                       = 662,
    ANIMKIT_GARROSH_2                       = 595
};

// Grand Executor Mortuus - 44615
struct npc_silverpine_grand_executor_mortuus : public ScriptedAI
{
    npc_silverpine_grand_executor_mortuus(Creature* creature) : ScriptedAI(creature), _summons(me), _eventInProgress(false) {}

    void OnQuestAccept(Player* /*player*/, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_THE_WARCHIEF_COMETH)
        {
            if (_eventInProgress)
                return;

            DoAction(ACTION_START_SCENE_COMETH);
        }
    }

    void Reset() override
    {
        _events.Reset();
        _garroshGUID.Clear();
        _cromushGUID.Clear();
        _sylvanasGUID.Clear();
        _agathaGUID.Clear();
        _summons.clear();
        _eventInProgress = false;
    }

    void JustSummoned(Creature* summon) override
    {
        _summons.Summon(summon);

        switch (summon->GetEntry())
        {
            case NPC_GARROSH_HELLSCREAM:
                summon->CastSpell(summon, SPELL_SIMPLE_TELEPORT);
                _garroshGUID = summon->GetGUID();
                break;

            case NPC_HELLSCREAM_ELITE_COMETH:
                summon->CastSpell(summon, SPELL_SIMPLE_TELEPORT);
                break;

            default:
                break;
        }
    }

    void DoAction(int32 param) override
    {
        switch (param)
        {
            case ACTION_START_SCENE_COMETH:
            {
                _eventInProgress = true;

                if (Creature* sylvanas = me->FindNearestCreature(NPC_LADY_SYLVANAS_WINDRUNNER_COMETH, 100.0f))
                {
                    _sylvanasGUID = sylvanas->GetGUID();

                    if (Creature* agatha = me->FindNearestCreature(NPC_AGATHA_COMETH, 100.0f))
                    {
                        _agathaGUID = agatha->GetGUID();

                        _events.ScheduleEvent(EVENT_START_SCENE_COMETH, 250ms);
                    }
                }

                break;
            }

            default:
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_START_SCENE_COMETH:
                    _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH, 1s);
                    _events.ScheduleEvent(EVENT_SUMMON_PORTAL_COMETH, 4s);
                    _events.ScheduleEvent(EVENT_SUMMON_GARROSH_COMETH, 7s + 500ms);
                    break;

                case EVENT_SUMMON_PORTAL_COMETH:
                    SummonPortalsFromOrgrimmar();
                    break;

                case EVENT_SUMMON_GARROSH_COMETH:
                    SummonGarroshAndHisEliteGuards();
                    break;

                case EVENT_SCENE_TALK_COMETH:
                {
                    if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanasGUID))
                    {
                        if (sylvanas->IsAIEnabled())
                            sylvanas->AI()->Talk(TALK_SYLVANAS_COMETH_0);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 1, 4s + 500ms);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 1:
                {
                    if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanasGUID))
                    {
                        sylvanas->SetFacingTo(0.808979f);

                        if (sylvanas->IsAIEnabled())
                            sylvanas->AI()->Talk(TALK_SYLVANAS_COMETH_1);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 2, 3s + 500ms);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 2:
                {
                    if (Creature* garrosh = ObjectAccessor::GetCreature(*me, _garroshGUID))
                    {
                        garrosh->GetMotionMaster()->MoveJump(GarroshJumpPos, 15.595897f, 15.595897f);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 3, 2s + 500ms);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 3:
                {
                    if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanasGUID))
                    {
                        sylvanas->SetFacingTo(3.924652f);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 4, 1s);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 4:
                {
                    if (Creature* garrosh = ObjectAccessor::GetCreature(*me, _garroshGUID))
                    {
                        if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanasGUID))
                            garrosh->SetFacingToObject(sylvanas);

                        garrosh->PlayOneShotAnimKitId(ANIMKIT_GARROSH_1);

                        if (garrosh->IsAIEnabled())
                            garrosh->AI()->Talk(TALK_GARROSH_COMETH_0);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 5, 12s);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 5:
                {
                    if (Creature* garrosh = ObjectAccessor::GetCreature(*me, _garroshGUID))
                    {
                        garrosh->SetFacingTo(3.9444442f);

                        if (garrosh->IsAIEnabled())
                            garrosh->AI()->Talk(TALK_GARROSH_COMETH_1);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 6, 7s);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 6:
                {
                    if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanasGUID))
                    {
                        sylvanas->SetFacingTo(2.4260077f);

                        if (sylvanas->IsAIEnabled())
                            sylvanas->AI()->Talk(TALK_SYLVANAS_COMETH_2);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 7, 5s);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 7:
                {
                    if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanasGUID))
                    {
                        sylvanas->SetFacingTo(3.7350047f);
                        sylvanas->PlayOneShotAnimKitId(ANIMKIT_SYLV_1);

                        if (sylvanas->IsAIEnabled())
                            sylvanas->AI()->Talk(TALK_SYLVANAS_COMETH_3);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 8, 16s);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 8:
                {
                    if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanasGUID))
                    {
                        if (sylvanas->IsAIEnabled())
                            sylvanas->AI()->Talk(TALK_SYLVANAS_COMETH_4);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 9, 4s);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 9:
                {
                    if (Creature* garrosh = ObjectAccessor::GetCreature(*me, _garroshGUID))
                    {
                        if (garrosh->IsAIEnabled())
                            garrosh->AI()->Talk(TALK_GARROSH_COMETH_2);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 10, 3s);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 10:
                {
                    if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanasGUID))
                    {
                        if (sylvanas->IsAIEnabled())
                            sylvanas->AI()->Talk(TALK_SYLVANAS_COMETH_5);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 11, 6s);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 11:
                {
                    if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanasGUID))
                    {
                        if (sylvanas->IsAIEnabled())
                            sylvanas->AI()->Talk(TALK_SYLVANAS_COMETH_6);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 12, 6s);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 12:
                {
                    if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanasGUID))
                    {
                        sylvanas->PlayOneShotAnimKitId(ANIMKIT_SYLV_2);

                        if (sylvanas->IsAIEnabled())
                            sylvanas->AI()->Talk(TALK_SYLVANAS_COMETH_7);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 13, 9s);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 13:
                {
                    if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanasGUID))
                    {
                        if (sylvanas->IsAIEnabled())
                            sylvanas->AI()->Talk(TALK_SYLVANAS_COMETH_8);

                        _events.ScheduleEvent(EVENT_AGATHA_RAISE_FORSAKEN, 3s);
                    }
                    break;
                }

                case EVENT_AGATHA_RAISE_FORSAKEN:
                {
                    if (Creature* agatha = ObjectAccessor::GetCreature(*me, _agathaGUID))
                    {
                        agatha->GetMotionMaster()->MovePoint(POINT_AGATHA_PRE_RISE, AgathaPreRisePos, false);

                        _events.ScheduleEvent(EVENT_AGATHA_RAISE_FORSAKEN + 1, 2s + 500ms);
                    }
                    break;
                }

                case EVENT_AGATHA_RAISE_FORSAKEN + 1:
                {
                    if (Creature* agatha = ObjectAccessor::GetCreature(*me, _agathaGUID))
                    {
                        agatha->SetWalk(true);
                        agatha->GetMotionMaster()->MovePoint(POINT_AGATHA_RISE, AgathaRisePos, false);

                        _events.ScheduleEvent(EVENT_AGATHA_RAISE_FORSAKEN + 2, 6s);
                    }
                    break;
                }

                case EVENT_AGATHA_RAISE_FORSAKEN + 2:
                {
                    if (Creature* agatha = ObjectAccessor::GetCreature(*me, _agathaGUID))
                    {
                        agatha->CastSpell(agatha, SPELL_RAISE_FORSAKEN_COMETH);

                        _events.ScheduleEvent(EVENT_AGATHA_RAISE_FORSAKEN + 3, 10s);
                    }
                    break;
                }

                case EVENT_AGATHA_RAISE_FORSAKEN + 3:
                {
                    if (Creature* agatha = ObjectAccessor::GetCreature(*me, _agathaGUID))
                    {
                        agatha->GetMotionMaster()->MovePoint(POINT_AGATHA_PRE_RESET, AgathaPreResetPos, false, 0.855211f);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 14, 750ms);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 14:
                {
                    if (Creature* cromush = ObjectAccessor::GetCreature(*me, _cromushGUID))
                    {
                        if (cromush->IsAIEnabled())
                            cromush->AI()->Talk(TALK_CROMUSH_COMETH_0);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 15, 3s + 500ms);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 15:
                {
                    if (Creature* garrosh = ObjectAccessor::GetCreature(*me, _garroshGUID))
                    {
                        garrosh->PlayOneShotAnimKitId(ANIMKIT_GENERAL_1);

                        if (garrosh->IsAIEnabled())
                            garrosh->AI()->Talk(TALK_GARROSH_COMETH_3);

                        if (Creature* agatha = ObjectAccessor::GetCreature(*me, _agathaGUID))
                            agatha->SetWalk(false);

                        _events.ScheduleEvent(EVENT_AGATHA_RAISE_FORSAKEN + 4, 12s);
                    }
                    break;
                }

                case EVENT_AGATHA_RAISE_FORSAKEN + 4:
                {
                    if (Creature* agatha = ObjectAccessor::GetCreature(*me, _agathaGUID))
                    {
                        agatha->GetMotionMaster()->MovePoint(POINT_AGATHA_RESET, AgathaResetPos, false, 0.7155f);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 16, 1s);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 16:
                {
                    if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanasGUID))
                    {
                        sylvanas->PlayOneShotAnimKitId(ANIMKIT_GENERAL_1);

                        if (sylvanas->IsAIEnabled())
                            sylvanas->AI()->Talk(TALK_SYLVANAS_COMETH_9);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 17, 10s);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 17:
                {
                    if (Creature* garrosh = ObjectAccessor::GetCreature(*me, _garroshGUID))
                    {
                        if (garrosh->IsAIEnabled())
                            garrosh->AI()->Talk(TALK_GARROSH_COMETH_4);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 18, 6s);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 18:
                {
                    if (Creature* garrosh = ObjectAccessor::GetCreature(*me, _garroshGUID))
                    {
                        if (garrosh->IsAIEnabled())
                            garrosh->AI()->Talk(TALK_GARROSH_COMETH_5);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 19, 6s);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 19:
                {
                    if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanasGUID))
                    {
                        if (sylvanas->IsAIEnabled())
                            sylvanas->AI()->Talk(TALK_SYLVANAS_COMETH_10);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 20, 5s);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 20:
                {
                    if (Creature* garrosh = ObjectAccessor::GetCreature(*me, _garroshGUID))
                    {
                        garrosh->SetFacingTo(5.51524f);

                        if (garrosh->IsAIEnabled())
                            garrosh->AI()->Talk(TALK_GARROSH_COMETH_6);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 21, 4s + 500ms);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 21:
                {
                    if (Creature* garrosh = ObjectAccessor::GetCreature(*me, _garroshGUID))
                    {
                        if (Creature* cromush = ObjectAccessor::GetCreature(*me, _cromushGUID))
                        {
                            garrosh->SetFacingToObject(cromush);

                            _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 22, 500ms);
                        }
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 22:
                {
                    if (Creature* garrosh = ObjectAccessor::GetCreature(*me, _garroshGUID))
                    {
                        garrosh->PlayOneShotAnimKitId(ANIMKIT_GARROSH_2);

                        if (garrosh->IsAIEnabled())
                            garrosh->AI()->Talk(TALK_GARROSH_COMETH_7);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 23, 14s);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 23:
                {
                    if (Creature* cromush = ObjectAccessor::GetCreature(*me, _cromushGUID))
                    {
                        if (cromush->IsAIEnabled())
                            cromush->AI()->Talk(TALK_CROMUSH_COMETH_1);

                        if (Creature* garrosh = ObjectAccessor::GetCreature(*me, _garroshGUID))
                        {
                            cromush->SetFacingToObject(garrosh);

                            _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 24, 2s + 500ms);
                        }
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 24:
                {
                    if (Creature* garrosh = ObjectAccessor::GetCreature(*me, _garroshGUID))
                    {
                        garrosh->SetFacingTo(5.6199603f);
                        garrosh->PlayOneShotAnimKitId(ANIMKIT_GARROSH_2);

                        if (garrosh->IsAIEnabled())
                            garrosh->AI()->Talk(TALK_GARROSH_COMETH_8);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 25, 8s + 500ms);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 25:
                {
                    if (Creature* garrosh = ObjectAccessor::GetCreature(*me, _garroshGUID))
                    {
                        garrosh->CastSpell(garrosh, SPELL_WELCOME_TO_SILVERPINE_CREDIT, true);

                        garrosh->GetMotionMaster()->MovePath(PATH_GARROSH, false);

                        _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 26, 9s);
                    }
                    break;
                }

                case EVENT_SCENE_TALK_COMETH + 26:
                    DespawnGarroshAndHisEliteGuards();
                    _events.ScheduleEvent(EVENT_SCENE_TALK_COMETH + 27, 500ms);
                    break;

                case EVENT_SCENE_TALK_COMETH + 27:
                    _summons.DespawnAll();
                    Reset();
                    break;

                default:
                    break;
            }
        }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

    void SummonPortalsFromOrgrimmar()
    {
        for (Position const& pos : OrgrimmarPortalPos)
            me->SummonCreature(NPC_PORTAL_FROM_ORGRIMMAR, pos, TEMPSUMMON_TIMED_DESPAWN, 300s);

        std::list<Creature*> orgrimmarPortals;
        GetCreatureListWithEntryInGrid(orgrimmarPortals, me, NPC_PORTAL_FROM_ORGRIMMAR, 100.0f);

        for (Creature* portal : orgrimmarPortals)
            portal->CastSpell(portal, SPELL_AIR_REVENANT_ENTRANCE);
    }

    void SummonGarroshAndHisEliteGuards()
    {
        for (Position const& pos : HellscreamElitePos)
            me->SummonCreature(NPC_HELLSCREAM_ELITE_COMETH, pos, TEMPSUMMON_TIMED_DESPAWN, 300s);

        me->SummonCreature(NPC_GARROSH_HELLSCREAM, GarroshPos, TEMPSUMMON_TIMED_DESPAWN, 300s);

        if (Creature* cromush = me->SummonCreature(NPC_HIGH_WARLORD_CROMUSH_COMETH, CromushPos, TEMPSUMMON_TIMED_DESPAWN, 300s))
        {
            _cromushGUID = cromush->GetGUID();

            cromush->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
            cromush->GetMotionMaster()->MovePath(PATH_CROMUSH, false);
        }
    }

    void DespawnGarroshAndHisEliteGuards()
    {
        for (ObjectGuid const& summonedCreature : _summons)
        {
            if (Creature* summon = ObjectAccessor::GetCreature(*me, summonedCreature))
                summon->CastSpell(summon, SPELL_SIMPLE_TELEPORT);
        }
    }

private:
    EventMap _events;
    ObjectGuid _garroshGUID;
    ObjectGuid _cromushGUID;
    ObjectGuid _sylvanasGUID;
    ObjectGuid _agathaGUID;
    SummonList _summons;
    bool _eventInProgress;
};

enum RaiseForsakenCometh
{
    ACTION_RISE_DURING_RAISE                = 1,
    ACTION_DESCEND_AFTER_RAISE              = 2,

    POINT_BEING_RISEN                       = 1,

    ANIMKIT_RESET                           = 0,
    ANIMKIT_FALLEN_HUMAN                    = 721
};

// Raise Forsaken - 83173
class spell_silverpine_raise_forsaken_83173 : public AuraScript
{
    PrepareAuraScript(spell_silverpine_raise_forsaken_83173);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();

        if (Creature* fallenHuman = target->ToCreature())
        {
            if (fallenHuman->IsAIEnabled())
                fallenHuman->AI()->DoAction(ACTION_RISE_DURING_RAISE);
        }
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();

        if (Creature* fallenHuman = target->ToCreature())
        {
            if (fallenHuman->IsAIEnabled())
                fallenHuman->AI()->DoAction(ACTION_DESCEND_AFTER_RAISE);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_silverpine_raise_forsaken_83173::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_silverpine_raise_forsaken_83173::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

enum FallenHuman
{
    SPELL_FEIGNED                               = 80636,
    SPELL_FORSAKEN_TROOPER_MS_COMETH            = 83149,

    EVENT_ASCEND                                = 1,
    EVENT_TRANSFORM_INTO_FORSAKEN               = 2,
    EVENT_FACE_TOWARDS_SYLVANAS                 = 3,
    EVENT_EMOTE_TO_SYLVANAS                     = 4
};

// Fallen Human - 44592, 44593
struct npc_silverpine_fallen_human : public ScriptedAI
{
    npc_silverpine_fallen_human(Creature* creature) : ScriptedAI(creature), _transformDone(false) {}

    void Reset() override
    {
        _transformDone = false;
        _events.Reset();
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_RISE_DURING_RAISE:
                me->SetAIAnimKitId(ANIMKIT_FALLEN_HUMAN);
                _events.ScheduleEvent(EVENT_ASCEND, 1s);
                break;

            case ACTION_DESCEND_AFTER_RAISE:
                me->SetWalk(false);
                me->SetAIAnimKitId(ANIMKIT_RESET);
                me->GetMotionMaster()->MoveFall();
                _events.ScheduleEvent(EVENT_TRANSFORM_INTO_FORSAKEN, 1s);
                break;

            default:
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_ASCEND:
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(POINT_BEING_RISEN, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 3.5f, false);
                    break;

                case EVENT_TRANSFORM_INTO_FORSAKEN:
                {
                    if (_transformDone)
                        return;

                    DoCastSelf(SPELL_FORSAKEN_TROOPER_MS_COMETH);

                    _transformDone = true;

                    _events.ScheduleEvent(EVENT_FACE_TOWARDS_SYLVANAS, 1s + 500ms);
                    break;
                }

                case EVENT_FACE_TOWARDS_SYLVANAS:
                    me->SetFacingTo(0.706837f);
                    _events.ScheduleEvent(EVENT_EMOTE_TO_SYLVANAS, 2s + 500ms);
                    break;

                case EVENT_EMOTE_TO_SYLVANAS:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                    me->DespawnOrUnsummon(80s);
                    break;

                default:
                    break;
            }
        }
    }

private:
    EventMap _events;
    bool _transformDone;
};

enum SpellForsakenTrooperMasterScriptCometh
{
    SPELL_FORSAKEN_TROOPER_MALE_01_HC       = 83150,
    SPELL_FORSAKEN_TROOPER_MALE_02_HC       = 83163,
    SPELL_FORSAKEN_TROOPER_MALE_03_HC       = 83164,
    SPELL_FORSAKEN_TROOPER_MALE_04_HC       = 83165,
    SPELL_FORSAKEN_TROOPER_FEMALE_01_HC     = 83152,
    SPELL_FORSAKEN_TROOPER_FEMALE_02_HC     = 83166,
    SPELL_FORSAKEN_TROOPER_FEMALE_03_HC     = 83167,
    SPELL_FORSAKEN_TROOPER_FEMALE_04_HC     = 83168,

    DISPLAY_MALE_01_HC                      = 33978,
    DISPLAY_MALE_02_HC                      = 33980,
    DISPLAY_MALE_03_HC                      = 33979,
    DISPLAY_MALE_04_HC                      = 33981,
    DISPLAY_FEMALE_01_HC                    = 33982,
    DISPLAY_FEMALE_02_HC                    = 33983,
    DISPLAY_FEMALE_03_HC                    = 33984,
    DISPLAY_FEMALE_04_HC                    = 33985
};

// Forsaken Trooper Master Script (Forsaken High Command) - 83149
class spell_silverpine_forsaken_trooper_masterscript_high_command : public SpellScript
{
    PrepareSpellScript(spell_silverpine_forsaken_trooper_masterscript_high_command);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo
        ({
            SPELL_FORSAKEN_TROOPER_MALE_01_HC,
            SPELL_FORSAKEN_TROOPER_MALE_02_HC,
            SPELL_FORSAKEN_TROOPER_MALE_03_HC,
            SPELL_FORSAKEN_TROOPER_MALE_04_HC,
            SPELL_FORSAKEN_TROOPER_FEMALE_01_HC,
            SPELL_FORSAKEN_TROOPER_FEMALE_02_HC,
            SPELL_FORSAKEN_TROOPER_FEMALE_03_HC,
            SPELL_FORSAKEN_TROOPER_FEMALE_04_HC
        });
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            caster->RemoveAura(SPELL_FEIGNED);

            uint32 spellId = SPELL_FORSAKEN_TROOPER_MALE_01_HC;
            switch (caster->GetDisplayId())
            {
                case DISPLAY_MALE_01_HC:
                    spellId = SPELL_FORSAKEN_TROOPER_MALE_01_HC;
                    break;
                case DISPLAY_MALE_02_HC:
                    spellId = SPELL_FORSAKEN_TROOPER_MALE_02_HC;
                    break;
                case DISPLAY_MALE_03_HC:
                    spellId = SPELL_FORSAKEN_TROOPER_MALE_03_HC;
                    break;
                case DISPLAY_MALE_04_HC:
                    spellId = SPELL_FORSAKEN_TROOPER_MALE_04_HC;
                    break;
                case DISPLAY_FEMALE_01_HC:
                    spellId = SPELL_FORSAKEN_TROOPER_FEMALE_01_HC;
                    break;
                case DISPLAY_FEMALE_02_HC:
                    spellId = SPELL_FORSAKEN_TROOPER_FEMALE_02_HC;
                    break;
                case DISPLAY_FEMALE_03_HC:
                    spellId = SPELL_FORSAKEN_TROOPER_FEMALE_03_HC;
                    break;
                case DISPLAY_FEMALE_04_HC:
                    spellId = SPELL_FORSAKEN_TROOPER_FEMALE_04_HC;
                    break;
                default:
                    break;
            }
            caster->CastSpell(caster, spellId, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_silverpine_forsaken_trooper_masterscript_high_command::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

void AddSC_silverpine_forest()
{
    RegisterCreatureAI(npc_silverpine_grand_executor_mortuus);
    RegisterSpellScript(spell_silverpine_raise_forsaken_83173);
    RegisterCreatureAI(npc_silverpine_fallen_human);
    RegisterSpellScript(spell_silverpine_forsaken_trooper_masterscript_high_command);
}
