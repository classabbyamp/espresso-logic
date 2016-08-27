var EspressoLogicMinimizer = require('bindings')('EspressoLogicMinimizer');

function doMinimization(data) {
  var badContent;

  if (!Array.isArray(data)) {
    throw 'EspressoLogicMinimizer: expected an array, got a ' + typeof data;
  }

  badContent = data.filter((element) => typeof element != 'string');

  if (badContent.length > 0) {
    throw 'EspressoLogicMinimizer: incorrect data content. Only strings are supported: ' + badContent;
  }

  return EspressoLogicMinimizer.minimize(data);
}

module.exports = doMinimization;
