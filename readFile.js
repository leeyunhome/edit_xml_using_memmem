const fs = require("fs");

fs.readFile("./org.gnome.Vino.gschema.xml", (err, data) => {
  if (err) {
    throw err;
  }
  console.log(data);
  console.log(data.toString());
});
