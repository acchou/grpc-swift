/*
 *
 * Copyright 2016, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "internal.h"
#include "cgrpc.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

void cgrpc_call_destroy(cgrpc_call *call) {
  grpc_call_destroy(call->call);
  free(call);
}

void cgrpc_call_reserve_space_for_operations(cgrpc_call *call, int max_operations) {
  call->ops = (grpc_op *) malloc(max_operations * sizeof(grpc_op));
  memset(call->ops, 0, max_operations * sizeof(grpc_op));
  call->ops_count = 0;
}

void cgrpc_call_add_operation(cgrpc_call *call, cgrpc_observer *observer) {
  grpc_op *op = &(call->ops[call->ops_count]);
  cgrpc_observer_apply(observer, op);
  call->ops_count++;
}

grpc_call_error cgrpc_call_perform(cgrpc_call *call, long tag) {
  grpc_call_error error = grpc_call_start_batch(call->call,
                                                call->ops,
                                                call->ops_count,
                                                cgrpc_create_tag(tag),
                                                NULL);
  return error;
}
