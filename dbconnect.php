<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "medicinali";

// Crea connessione
$conn = new mysqli($servername, $username, $password, $dbname);

// Controlla connessione
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Ottieni i dati dalla richiesta POST
$temperature = isset($_POST['temperature']) ? $_POST['temperature'] : '';
$scompartimento = isset($_POST['scompartimento']) ? $_POST['scompartimento'] : '';

if ($temperature !== '' && $scompartimento!== '') {
    // Aggiorna il campo temp_media nella tabella med
    $sql = "UPDATE med SET temp_media = '$temperature' WHERE scompartimento = '$scompartimento'"; // Assumi che ci sia un record con id = 1
    if ($conn->query($sql) === TRUE) {
        echo "Temperatura aggiornata con successo";
    } else {
        echo "Errore nell'aggiornamento della temperatura: " . $conn->error;
    }
} else {
    // Ottieni orari e pillole rimanenti per entrambi gli scompartimenti
    $sql = "SELECT orario_impostato, numero_pillole_rimanenti FROM med";
    $result = $conn->query($sql);

    if ($result->num_rows > 0) {
        $rows = array();
        while($row = $result->fetch_assoc()) {
            $rows[] = $row;
        }
        echo json_encode($rows);
    } else {
        echo "Nessun risultato";
    }
}

$conn->close();
?>
