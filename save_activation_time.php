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
$orario_assunzione = isset($_POST['orario_assunzione']) ? $_POST['orario_assunzione'] : '';
$scompartimento = isset($_POST['scompartimento']) ? $_POST['scompartimento'] : '';

if ($orario_assunzione !== '' && $scompartimento !== '') {
    $sql = "UPDATE med SET orario_assunzione = '$orario_assunzione',numero_pillole_rimanenti=numero_pillole_rimanenti-1 WHERE scompartimento = '$scompartimento'";
    if ($conn->query($sql) === TRUE) {
        echo "Orario di attivazione salvato con successo";
    } else {
        echo "Errore nel salvataggio dell'orario di attivazione: " . $conn->error;
    }
} else {
    echo "Dati mancanti";
}

$conn->close();
?>
