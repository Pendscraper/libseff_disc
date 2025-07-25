#
# Copyright (c) 2023 Huawei Technologies Co., Ltd.
#
# libseff is licensed under Mulan PSL v2.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
# 	    http://license.coscl.org.cn/MulanPSL2
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
# FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v2 for more details.
#
# libseff declarations that will be used by both C and assembly components
#

from generate import *
import generate
import argparse

parser = argparse.ArgumentParser()

x86_64 = 'x86-64'
parser.add_argument('--arch', choices=[x86_64], default=x86_64)

segmented = 'segmented'
fixed = 'fixed'
vmmem = 'vmmem'
parser.add_argument('--stack', choices=[segmented, fixed, vmmem], default=segmented)

id_stackref = 'stackref'
id_counter = 'counter'
parser.add_argument('--effid_assign', choices=[id_stackref, id_counter], default=id_stackref)

parser.add_argument('--sequence-counters', action='store_true')

args = parser.parse_args()

if args.sequence_counters:
    print(f"Fatal error: --sequence-counters not implemented yet")
    exit()

if args.arch == x86_64:
    arch = Architecture(64)
    generate.arch = arch
else:
    print(f"Fatal error: unsupported architecture {args.arch}")
    exit()

stack_policy = args.stack
if stack_policy == segmented:
    Defn('STACK_POLICY_SEGMENTED')
    Defn('STACK_POLICY_SWITCH(segmented, fixed, vmmem)', 'segmented')
elif stack_policy == fixed:
    Defn('STACK_POLICY_FIXED')
    Defn('STACK_POLICY_SWITCH(segmented, fixed, vmmem)', 'fixed')
elif stack_policy == vmmem:
    Defn('STACK_POLICY_VM')
    Defn('STACK_POLICY_SWITCH(segmented, fixed, vmmem)', 'vmmem')
    print(f'Fatal error: stack policy {stack_policy} is currently unsupported')
else:
    print(f'Fatal error: unsupported stack policy {stack_policy}')

id_policy = args.effid_assign
if id_policy == id_stackref:
    Defn('EFF_ID_POLICY_STACKREF')
    Defn('EFF_ID_POLICY_SWITCH(stackref, counter)', 'stackref')
elif id_policy == id_counter:
    Defn('EFF_ID_POLICY_COUNTER')
    Defn('EFF_ID_POLICY_SWITCH(stackref, counter)', 'counter')
else:
    print(f'Fatal error: unsupported effect ID generation policy {id_policy}')


default_handler = Typedef("default_handler_t", func([ptr(void)], ptr(void)))
effect_id = Typedef('effect_id', ptr(default_handler.ty))
effect_set = Typedef("effect_set", ptr(effect_id.ty))

genname = "effect_id_generative_cell"
pt = ptr(unsized_named_ty("struct _" + genname))
gen_effect_set_cell = Struct(genname, Field("previous", pt), Field("handler", ptr(default_handler.ty)), Field("next", pt))

eff = Struct('seff_request_t',
    Field('effect', effect_id.ty),
    Field('payload', ptr(void))
)

Defn('RETURN_EFFECT_ID', '0x0')

cont_fields = [
    Field('current_coroutine', ptr(void)),

    Field('rsp', ptr(void)),
    Field('rbp', ptr(void)),

    Field('ip', ptr(void)),
    Field('rbx', ptr(void)),
    Field('r12', ptr(void)),
    Field('r13', ptr(void)),
    Field('r14', ptr(void)),
    Field('r15', ptr(void)),
]
if stack_policy == segmented:
    cont_fields.insert(0, Field('stack_top', ptr(void)))
cont = Struct('seff_cont_t', *cont_fields)

coroutine_state = Enum('seff_coroutine_state_t',
    'FINISHED',
    'PAUSED',
    'RUNNING'
)

# /* When we allocate a stack segment we put this header at the
#    start.  */

stack_ptr = None
if stack_policy == segmented:
    stack_segment = Struct('seff_stack_segment_t',
        Field('prev', ptr(unsized_named_ty('struct _seff_stack_segment_t'))), # This should be improved
        Field('next', ptr(unsized_named_ty('struct _seff_stack_segment_t'))), # This should be improved
        Field('size', arch.size_t),
        Field('canary', ptr(void)),
        # Padding to allow __morestack to run and to host the red zone mandated by
        # the x64 ABI
        # TODO: figure out if we can reduce this
        Field('padding', arr(byte, 128)),
    )
    frame_ptr = Typedef('seff_frame_ptr_t', ptr(stack_segment.ty))
else:
    frame_ptr = Typedef('seff_frame_ptr_t', ptr(void))

coroutine = Struct('seff_coroutine_t',
    Field('frame_ptr', frame_ptr.ty),
    Field('resume_point', cont.ty),
    Field('state', coroutine_state.ty),
    Field('parent_coroutine', ptr(unsized_named_ty('struct _seff_coroutine_t'))), # This should be improved
    Field('handled_effects', effect_set.ty)
)

generate_file(__file__)
