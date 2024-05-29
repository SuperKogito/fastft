var chmod_dict = {
  "r": 4,
  "w": 2,
  "x": 1,
  "rwx": "7",
  "rw-": "6",
  "r-x": "5",
  "r--": "4",
  "-wx": "3",
  "-w-": "2",
  "--x": "1",
  "---": "0",
  "7": "rwx",
  "6": "rw-",
  "5": "r-x",
  "4": "r--",
  "3": "-wx",
  "2": "-w-",
  "1": "--x",
  "0": "---"
}
var chmod_sub_symbolics = ["rwx", "rw-", "r-x", "r--", "-wx", "-w-", "--x", "---"]


function symbolic2table(symbolic) {
  document.chmod.ownerr.checked = "r" == symbolic[1];
  document.chmod.ownerw.checked = "w" == symbolic[2];
  document.chmod.ownerx.checked = "x" == symbolic[3];

  document.chmod.groupr.checked = "r" == symbolic[4];
  document.chmod.groupw.checked = "w" == symbolic[5];
  document.chmod.groupx.checked = "x" == symbolic[6];

  document.chmod.otherr.checked = "r" == symbolic[7];
  document.chmod.otherw.checked = "w" == symbolic[8];
  document.chmod.otherx.checked = "x" == symbolic[9];
}


function chmod2table() {
  var symbolic = "-";
  if (document.chmod.ownerr.checked) {
    symbolic += "r";
  } else {
    symbolic += "-";
  }
  if (document.chmod.ownerw.checked) {
    symbolic += "w";
  } else {
    symbolic += "-";
  }
  if (document.chmod.ownerx.checked) {
    symbolic += "x";
  } else {
    symbolic += "-";
  }

  if (document.chmod.groupr.checked) {
    symbolic += "r";
  } else {
    symbolic += "-";
  }
  if (document.chmod.groupw.checked) {
    symbolic += "w";
  } else {
    symbolic += "-";
  }
  if (document.chmod.groupx.checked) {
    symbolic += "x";
  } else {
    symbolic += "-";
  }

  if (document.chmod.otherr.checked) {
    symbolic += "r";
  } else {
    symbolic += "-";
  }
  if (document.chmod.otherw.checked) {
    symbolic += "w";
  } else {
    symbolic += "-";
  }
  if (document.chmod.otherx.checked) {
    symbolic += "x";
  } else {
    symbolic += "-";
  }

  document.chmod.chmod_symbolic.value = symbolic;
  var owner = symbolic.substring(1, 4);
  var group = symbolic.substring(4, 7);
  var other = symbolic.substring(7, symbolic.length);
  document.chmod.chmod_numeric.value = chmod_dict[owner] + chmod_dict[group] + chmod_dict[other];
}


function numeric2symbolic() {
  // get value
  var numeric = document.chmod.chmod_numeric.value;
  var len = numeric.length;

  // padding
  while ((len < 3) && (numeric != "")) {
    numeric = "0" + numeric;
    len = numeric.length;
  }
  console.log(numeric)

  // converting
  if (!numeric.includes("8") && !numeric.includes("9") && (len == 3)) {
      var symbolic = "";
      for (var i = 0; i < len; i++) {
        symbolic += chmod_dict[numeric[i]];
      }

      console.log(symbolic);
      document.chmod.chmod_symbolic.value = "-" + symbolic;
      symbolic2table("-" + symbolic);
    } else {
      document.chmod.chmod_symbolic.value = "undefined";
      symbolic2table("----------");
    }
  }


  function symbolic2numeric() {
    // init values
    var symbolic = document.chmod.chmod_symbolic.value;
    var len = symbolic.length;
    var owner = symbolic.substring(1, 4);
    var group = symbolic.substring(4, 7);
    var other = symbolic.substring(7, len);
    console.log(symbolic);
    console.log(len);
    console.log(owner + "|" + group + "|" + other);


    // padding
    if ((len == 10) && chmod_sub_symbolics.includes(owner) && chmod_sub_symbolics.includes(group) && chmod_sub_symbolics.includes(other)) {
      console.log(chmod_dict[owner] + "|" + chmod_dict[group] + "|" + chmod_dict[other]);
      document.chmod.chmod_numeric.value = chmod_dict[owner] + chmod_dict[group] + chmod_dict[other];
      symbolic2table(symbolic);
    } else {
      document.chmod.chmod_numeric.value = "xxx";
      symbolic2table("----------");
    }
  }
  window.onload = numeric2symbolic;
