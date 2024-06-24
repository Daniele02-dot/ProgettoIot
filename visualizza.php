<!DOCTYPE html>
<html lang="it">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Visualizza Scompartimenti</title>
<style>
  body {
    font-family: 'Arial', sans-serif;
    background-color: #f4f4f4;
    margin: 0;
    padding: 0;
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
  }
  .container {
    display: flex;
    justify-content: space-around;
    width: 80%;
  }
  table {
    border-collapse: collapse;
    width: 45%;
  }
  th, td {
    border: 1px solid #ddd;
    padding: 8px;
    text-align: left;
  }
  th {
    background-color: #04AA6D;
    color: white;
  }
  .status {
    height: 20px;
    width: 20px;
    border-radius: 50%;
    display: inline-block;
  }
  .green {
    background-color: #4CAF50;
  }
  .red {
    background-color: #F44336;
  }
</style>
</head>
<body>

<div class="container">
  <?php
  $servername = "localhost"; // o il tuo server di database
  $username = "root"; // il tuo username per il database
  $password = ""; // la tua password per il database
  $dbname = "medicinali"; // il nome del tuo database

  // Crea connessione
  $conn = new mysqli($servername, $username, $password, $dbname);

  // Controlla la connessione
  if ($conn->connect_error) {
    die("Connessione fallita: " . $conn->connect_error);
  }

  // Funzione per generare tabella per uno specifico scompartimento
  function generateTable($conn, $scompartimento) {
    $sql = "SELECT nome, numero_pillole_rimanenti, temp_media, temp_ideale, orario_impostato, orario_assunzione FROM med WHERE scompartimento = ?";
    $stmt = $conn->prepare($sql);
    $stmt->bind_param("i", $scompartimento);
    $stmt->execute();
    $result = $stmt->get_result();

    echo '<table>';
    echo '<tr><th>Nome del medicinale</th><th>Numero pillole rimanenti</th><th>Temperatura media</th><th>Temperatura ideale</th><th>Orario impostato</th><th>Orario di assunzione</th></tr>';
    if ($result->num_rows > 0) {
      while($row = $result->fetch_assoc()) {
        echo "<tr><td>" . $row["nome"]. "</td><td>" . $row["numero_pillole_rimanenti"]. "</td><td>" . $row["temp_media"]. "</td><td>" . $row["temp_ideale"]. "</td><td>" . $row["orario_impostato"]. "</td><td>" . $row["orario_assunzione"]. "</td></tr>";
      }
    } else {
      echo "<tr><td colspan='6'>0 risultati</td></tr>";
    }
    echo '</table>';

    $stmt->close();
  }

  echo '<div>';
  echo '<h2>Scompartimento 1</h2>';
  generateTable($conn, 1);
  echo '</div>';

  echo '<div>';
  echo '<h2>Scompartimento 2</h2>';
  generateTable($conn, 2);
  echo '</div>';

  $conn->close();
  ?>
</div>

</body>
</html>
