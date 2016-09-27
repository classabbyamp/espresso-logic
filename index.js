var EspressoLogicMinimizer = require('bindings')('EspressoLogicMinimizer');

/**
 * Applies the Espresso Heuristic Logic Minimizer algorithm to the provided
 * data in PLA format
 *
 * @param {Array} data
 * @returns {Array}
 */
function doMinimization(data) {
  var badContent;

  if (!Array.isArray(data)) {
    throw new Error('EspressoLogicMinimizer: expected an array, got a ' + typeof data);
  }

  badContent = data.filter((element) => typeof element != 'string');

  if (badContent.length > 0) {
    throw new Error('EspressoLogicMinimizer: incorrect data content. Only strings are supported: ' + badContent);
  }

  return EspressoLogicMinimizer.minimize(data);
}

module.exports = doMinimization;
