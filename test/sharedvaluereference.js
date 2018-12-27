'use strict';
const buildType = process.config.target_defaults.default_configuration;
const assert = require('assert');

test(require(`./build/${buildType}/binding.node`));
test(require(`./build/${buildType}/binding_noexcept.node`));

function test(binding) {
  const {sharedvaluereference} = binding;
  assert.ok(sharedvaluereference.testEmptyIsEmpty());
  assert.ok(sharedvaluereference.testEmptyRefIsNullptr());
  assert.ok(sharedvaluereference.testEmptyValueIsEmpty());
  assert.ok(sharedvaluereference.testEmptyIsMovable());
  assert.ok(sharedvaluereference.testInitializedIsNotEmpty());
  assert.ok(sharedvaluereference.testInitializedRefIsNotNullptr());
  assert.ok(sharedvaluereference.testInitializedResetRefcountZero());
  assert.ok(sharedvaluereference.testInitializedIsMovable());
  assert.ok(sharedvaluereference.testCopyRefAreEqual());
  assert.ok(sharedvaluereference.testCopyResetRefcountOne());
  
  // flaky
  for (let i = 0; i < 20; ++i) {
    const threadCount = i * 10;
    assert.ok(sharedvaluereference.testRefcountResetInOtherThreads(threadCount));
  }
}
