'use strict';
const fs = require('fs');
const path = require('path');

process.config.target_defaults.default_configuration = require('./build/env.json').buildType;

const testSrc = path.join(__dirname, 'src');
const testModulesRegex = /\.test\.js$/;
const testModules = fs
  .readdirSync(testSrc)
  .filter(entry => testModulesRegex.test(entry))
  .map(entry => path.join(testSrc, entry))
;

if (typeof global.gc === 'function') {
  console.log('Starting test suite\n');

  // Requiring each module runs tests in the module.
  testModules.forEach(name => {
    const result = require(name);
    console.log(`Running test '${path.basename(name)}' ${(result && result.skipped) ? '[SKIPPED]' : ''}`);
  });

  console.log('\nAll tests passed!');
} else {
  // Make it easier to run with the correct (version-dependent) command-line args.
  const child = require('./napi_child').spawnSync(process.argv[0], [ '--expose-gc', __filename ], {
    stdio: 'inherit',
  });

  if (child.signal) {
    console.error(`Tests aborted with ${child.signal}`);
    process.exitCode = 1;
  } else {
    process.exitCode = child.status;
  }
  process.exit(process.exitCode);
}
