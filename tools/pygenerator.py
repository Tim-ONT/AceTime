# Copyright 2018 Brian T. Park
#
# MIT License
"""
Generate the zone_info and zone_policies files for Python.
"""

import logging
import os

from transformer import short_name
from argenerator import normalize_name
from argenerator import normalize_raw

class PythonGenerator:
    """Generate Python files for zone infos and policies.
    """

    ZONE_POLICIES_FILE = """\
# This file was generated by the following script:
#
#  $ {invocation}
#
# using the TZ Database files
#
#  {tz_files}
#
# from https://github.com/eggert/tz/releases/tag/{tz_version}
#
# DO NOT EDIT

# numPolicies: {numPolicies}
# numRules: {numRules}

{policyItems}

#---------------------------------------------------------------------------

# The following zone policies are not supported in the current version of
# AceTime.
#
# numPolicies: {numRemovedPolicies}
#
{removedPolicyItems}

#---------------------------------------------------------------------------

# The following zone policies may have inaccuracies due to the following
# reasons:
#
# numPolicies: {numNotablePolicies}
#
{notablePolicyItems}

"""

    ZONE_POLICY_ITEM = """\
#---------------------------------------------------------------------------
# Policy name: {policyName}
# Rule count: {numRules}
#---------------------------------------------------------------------------
ZONE_RULES_{policyName} = [
{ruleItems}
]
ZONE_POLICY_{policyName} = {{
    'name': '{policyName}',
    'rules': ZONE_RULES_{policyName}
}}

"""

    ZONE_REMOVED_POLICY_ITEM = """\
# {policyName} ({policyReason})
"""

    ZONE_NOTABLE_POLICY_ITEM = """\
# {policyName} ({policyReason})
"""


    ZONE_RULE_ITEM = """\
    # {rawLine}
    {{
        'fromYear': {fromYear},
        'toYear': {toYear},
        'inMonth': {inMonth},
        'onDayOfWeek': {onDayOfWeek},
        'onDayOfMonth': {onDayOfMonth},
        'atSeconds': {atSeconds},
        'atTimeModifier': '{atTimeModifier}',
        'deltaSeconds': {deltaSeconds},
        'letter': '{letter}',
    }},
"""

    ZONE_INFOS_FILE = """\
# This file was generated by the following script:
#
#  $ {invocation}
#
# using the TZ Database files
#
#  {tz_files}
#
# from https://github.com/eggert/tz/releases/tag/{tz_version}
#
# DO NOT EDIT

from zonedb.zone_policies import *

# numInfos: {numInfos}
# numEras: {numEras}

{infoItems}

#---------------------------------------------------------------------------

ZONE_INFO_MAP = {{
{infoMapItems}
}}

#---------------------------------------------------------------------------

# The following zones are not supported in the current version of AceTime.
#
# numInfos: {numRemovedInfos}
#
{removedInfoItems}

#---------------------------------------------------------------------------

# The following zones may have inaccuracies due to the following reasons:
#
# numInfos: {numNotableInfos}
#
{notableInfoItems}
"""

    ZONE_INFO_ITEM = """\
#---------------------------------------------------------------------------
# Zone name: {infoFullName}
# Era count: {numEras}
#---------------------------------------------------------------------------

ZONE_ERAS_{infoShortName} = [
{eraItems}
]
ZONE_INFO_{infoShortName} = {{
    'name': '{infoFullName}',
    'eras': ZONE_ERAS_{infoShortName}
}}

"""

    ZONE_REMOVED_INFO_ITEM = """\
# {infoFullName} ({infoReason})
"""

    ZONE_NOTABLE_INFO_ITEM = """\
# {infoFullName} ({infoReason})
"""

    ZONE_ERA_ITEM = """\
    # {rawLine}
    {{
      'offsetSeconds': {offsetSeconds},
      'zonePolicy': {zonePolicy},
      'rulesDeltaSeconds': {rulesDeltaSeconds},
      'format': '{format}',
      'untilYear': {untilYear},
      'untilMonth': {untilMonth},
      'untilDay': {untilDay},
      'untilSeconds': {untilSeconds},
      'untilTimeModifier': '{untilTimeModifier}',
    }},
"""

    ZONE_INFO_MAP_ITEM = """\
    "{infoShortName}": ZONE_INFO_{infoShortName}, # {infoFullName}
"""

    ZONE_INFOS_FILE_NAME = 'zone_infos.py'
    ZONE_POLICIES_FILE_NAME = 'zone_policies.py'

    def __init__(self, invocation, tz_version, tz_files,
                 zones_map, rules_map, removed_zones, removed_policies,
                 notable_zones, notable_policies):
        self.invocation = invocation
        self.tz_version = tz_version
        self.tz_files = tz_files
        self.zones_map = zones_map
        self.rules_map = rules_map
        self.removed_zones = removed_zones
        self.removed_policies = removed_policies
        self.notable_zones = notable_zones
        self.notable_policies = notable_policies

    def generate_files(self, output_dir):
        self.write_file(output_dir,
            self.ZONE_POLICIES_FILE_NAME, self.generate_policies())

        self.write_file(output_dir,
            self.ZONE_INFOS_FILE_NAME, self.generate_infos())

    def write_file(self, output_dir, filename, content):
        full_filename = os.path.join(output_dir, filename)
        with open(full_filename, 'w', encoding='utf-8') as output_file:
            print(content, end='', file=output_file)
        logging.info("Created %s", full_filename)

    def generate_policies(self):
        (num_rules, policy_items) = self.generate_policy_items(self.rules_map)
        removed_policy_items = self.generate_removed_policy_items(
            self.removed_policies)
        notable_policy_items = self.generate_notable_policy_items(
            self.notable_policies)

        return self.ZONE_POLICIES_FILE.format(
            invocation=self.invocation,
            tz_version=self.tz_version,
            tz_files=', '.join(self.tz_files),
            numPolicies=len(self.rules_map),
            numRules=num_rules,
            policyItems=policy_items,
            numRemovedPolicies=len(self.removed_policies),
            removedPolicyItems=removed_policy_items,
            numNotablePolicies=len(self.notable_policies),
            notablePolicyItems=notable_policy_items)

    def generate_policy_items(self, rules_map):
        num_rules = 0
        policy_items = ''
        for name, rules in sorted(rules_map.items()):
            policy_items += self.generate_policy_item(name, rules)
            num_rules += len(rules)
        return (num_rules, policy_items)

    def generate_policy_item(self, name, rules):
        rule_items = ''
        for rule in rules:
            rule_items += self.ZONE_RULE_ITEM.format(
                policyName=normalize_name(name),
                rawLine=normalize_raw(rule['rawLine']),
                fromYear=rule['fromYear'],
                toYear=rule['toYear'],
                inMonth=rule['inMonth'],
                onDayOfWeek=rule['onDayOfWeek'],
                onDayOfMonth=rule['onDayOfMonth'],
                atSeconds=rule['atSecondsTruncated'],
                atTimeModifier=rule['atTimeModifier'],
                deltaSeconds=rule['deltaSecondsTruncated'],
                letter=rule['letter'])
        return self.ZONE_POLICY_ITEM.format(
            policyName=normalize_name(name),
            numRules=len(rules),
            ruleItems=rule_items);

    def generate_removed_policy_items(self, removed_policies):
        removed_policy_items = ''
        for name, reason in sorted(removed_policies.items()):
            removed_policy_items += \
                self.ZONE_REMOVED_POLICY_ITEM.format(
                    policyName=normalize_name(name),
                    policyReason=reason)
        return removed_policy_items

    def generate_notable_policy_items(self, notable_policies):
        notable_policy_items = ''
        for name, reason in sorted(notable_policies.items()):
            notable_policy_items += \
                self.ZONE_NOTABLE_POLICY_ITEM.format(
                    policyName=normalize_name(name),
                    policyReason=reason)
        return notable_policy_items

    def generate_infos(self):
        (num_eras, info_items) = self.generate_info_items(self.zones_map)
        removed_info_items = self.generate_removed_info_items(
            self.removed_zones)
        notable_info_items = self.generate_notable_info_items(
            self.notable_zones)
        info_map_items = self.generate_info_map_items(self.zones_map)

        return self.ZONE_INFOS_FILE.format(
            invocation=self.invocation,
            tz_version=self.tz_version,
            tz_files=', '.join(self.tz_files),
            numInfos=len(self.zones_map),
            numEras=num_eras,
            infoItems=info_items,
            infoMapItems=info_map_items,
            numRemovedInfos=len(self.removed_zones),
            removedInfoItems=removed_info_items,
            numNotableInfos=len(self.notable_zones),
            notableInfoItems=notable_info_items)

    def generate_info_items(self, zones_map):
        info_items = ''
        num_eras = 0
        for name, eras in sorted(self.zones_map.items()):
            info_items += self.generate_info_item(name, eras)
            num_eras += len(eras)
        return (num_eras, info_items)

    def generate_removed_info_items(self, removed_zones):
        removed_info_items = ''
        for name, reason in sorted(removed_zones.items()):
            removed_info_items += self.ZONE_REMOVED_INFO_ITEM.format(
                infoFullName=name,
                infoReason=reason)
        return removed_info_items

    def generate_notable_info_items(self, notable_zones):
        notable_info_items = ''
        for name, reason in sorted(notable_zones.items()):
            notable_info_items += self.ZONE_NOTABLE_INFO_ITEM.format(
                infoFullName=name,
                infoReason=reason)
        return notable_info_items

    def generate_info_item(self, name, eras):
        era_items = ''
        for era in eras:
            era_items += self.generate_era_item(era)

        return self.ZONE_INFO_ITEM.format(
            infoFullName=normalize_name(name),
            infoShortName=normalize_name(short_name(name)),
            numEras=len(eras),
            eraItems=era_items)

    def generate_era_item(self, era):
        policy_name = era['rules']
        if policy_name in ['-', ':']:
            zone_policy = "'%s'" % policy_name
        else:
            zone_policy = 'ZONE_POLICY_%s' % normalize_name(policy_name)

        rules_delta_seconds = era['rulesDeltaSecondsTruncated']
        if not rules_delta_seconds: rules_delta_seconds = 0

        return self.ZONE_ERA_ITEM.format(
            rawLine=normalize_raw(era['rawLine']),
            offsetSeconds=era['offsetSecondsTruncated'],
            zonePolicy=zone_policy,
            rulesDeltaSeconds=rules_delta_seconds,
            format=era['format'], # preserve the %s
            untilYear=era['untilYear'],
            untilMonth=era['untilMonth'],
            untilDay=era['untilDay'],
            untilSeconds=era['untilSecondsTruncated'],
            untilTimeModifier=era['untilTimeModifier'])

    def generate_info_map_items(self, zones_map):
        """Generate a map of (shortName -> zoneInfo), shorted by shortName.
        """
        info_map_items = ''
        for name, zones in sorted(zones_map.items(),
            key=lambda x: normalize_name(short_name(x[0]))):
            info_map_items += self.ZONE_INFO_MAP_ITEM.format(
                infoShortName=normalize_name(short_name(name)),
                infoFullName=name)
        return info_map_items
