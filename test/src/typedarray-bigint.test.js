'use strict';

if ((process.env.npm_config_NAPI_VERSION !== undefined) && (process.env.npm_config_NAPI_VERSION < 50000)) {
  exports.skipped = true;
  return;
}

require('./typedarray-bigint.test.def');