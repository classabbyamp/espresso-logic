const
  EspressoLogicMinimizer = require('bindings')('EspressoLogicMinimizer'),
  crypto = require('crypto'),
  fs = require('fs'),
  tmp = require('tmp');

/**
 * Applies the Espresso Heuristic Logic Minimizer algorithm to the provided
 * data in PLA format
 *
 * @param {Array} data
 * @returns {Array}
 */
function minimize(data) {
  if (!Array.isArray(data)) {
    throw new Error('EspressoLogicMinimizer: expected an array, got a ' + typeof data);
  }

  const badContent = data.filter((element) => typeof element != 'string');

  if (badContent.length > 0) {
    throw new Error('EspressoLogicMinimizer: incorrect data content. Only strings are supported: ' + badContent);
  }

  return EspressoLogicMinimizer.minimize_from_data(data);
}

/**
 * @class Espresso
 */
class Espresso {
  /**
   * @throws
   * @param {number} input - number of input variables
   * @param {number} output - number of output variables
   */
  constructor(inputSize, outputSize) {
    this.result = null;
    this.tmpfile = tmp.fileSync();
    console.log('TMP FILE CREATED: ', this.tmpfile);
    fs.writeSync(this.tmpfile.fd, '.i ' + inputSize + '\n');
    fs.writeSync(this.tmpfile.fd, '.o ' + outputSize + '\n');
  }

  /**
   * Pushes a truth table line into the minimizer
   * @param {array.<number>} input
   * @param {array.<number>} output
   */
  push(input, output) {
    if (this.result) {
      return this.result;
    }

    fs.writeSync(this.tmpfile.fd,
      input.reduce((p, c) => p + (c ? '1' : '0'), '')
      + ' '
      + output.reduce((p, c) => p + (c ? '1' : '0'), '')
      + '\n'
    );
  }

  /**
   * Performs the minimization
   */
  minimize() {
    if (this.result) {
      return this.result;
    }

    fs.closeSync(this.tmpfile.fd);
    console.log('Providing file to espresso: ', this.tmpfile);
    console.log('EXISTS: ', fs.existsSync(this.tmpfile.name));
    this.result = EspressoLogicMinimizer.minimize_from_path(this.tmpfile.name);
    this.tmpfile.removeCallback();

    return this.result;
  }
}

module.exports = {
  minimize,
  Espresso
};
