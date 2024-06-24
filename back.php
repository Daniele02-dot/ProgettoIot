<?php
// Connessione al database
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "medicinali";

// Creare una connessione
$conn = new mysqli($servername, $username, $password, $dbname);

// Controllare la connessione
if ($conn->connect_error) {
  die("Connessione fallita: " . $conn->connect_error);
}

// Verificare se il form è stato inviato
if ($_SERVER["REQUEST_METHOD"] == "POST") {

  // Assegnare i dati del form alle variabili
  $nome_medicinale = $_POST['nome'];
  $numero_pillole = (int)$_POST['numero_pillole_rimanenti'];
  $temperatura_ideale = (double)$_POST['temp_ideale'];
  $orario_assunzione = 0; // Assicurati che sia gestito correttamente
  $data_scadenza = $_POST['data_scadenza'];
  $temp_media = 0; // Assicurati che sia gestito correttamente
  $orario_reale = $_POST['orario_reale'];
  $stato_assunzione = false; // Devi gestire o assegnare un valore appropriato
  $scompartimento = (int)$_POST['scompartimento']; // Recuperare il valore del campo scompartimento

  // Verificare se esiste già un record per lo scompartimento specificato
  $query = "SELECT * FROM med WHERE scompartimento = ?";
  $stmt = $conn->prepare($query);
  $stmt->bind_param("i", $scompartimento);
  $stmt->execute();
  $result = $stmt->get_result();

  if ($result->num_rows > 0) {
    // Record esistente, preparare la query di aggiornamento
    $stmt = $conn->prepare("UPDATE med SET nome = ?, numero_pillole_rimanenti = ?, temp_media = ?, orario_impostato = ?, orario_assunzione = ?, temp_ideale = ?, data_scadenza = ?, stato_assunzione = ? WHERE scompartimento = ?");
    $stmt->bind_param("siississi", $nome_medicinale, $numero_pillole, $temp_media, $orario_reale, $orario_assunzione, $temperatura_ideale, $data_scadenza, $stato_assunzione, $scompartimento);
  } else {
    // Nessun record esistente, preparare la query di inserimento
    $stmt = $conn->prepare("INSERT INTO med (nome, numero_pillole_rimanenti, temp_media, orario_impostato, orario_assunzione, temp_ideale, data_scadenza, stato_assunzione, scompartimento) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
    $stmt->bind_param("siississi", $nome_medicinale, $numero_pillole, $temp_media, $orario_reale, $orario_assunzione, $temperatura_ideale, $data_scadenza, $stato_assunzione, $scompartimento);
  }

  // Eseguire la query
  if ($stmt->execute()) {
    echo "Dati aggiornati con successo";
    
    // Reindirizzamento alla pagina di configurazione dopo 3 secondi
    header("Refresh: 3; URL=configura.html");
    exit; // Termina lo script PHP dopo il reindirizzamento
  } else {
    echo "Errore: " . $stmt->error;
  }

  // Chiudere la connessione
  $stmt->close();
  $conn->close();
} else {
  echo "Errore: nessun dato registrato";
}
?>
