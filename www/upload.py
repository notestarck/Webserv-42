# Code genere avec openai
from flask import Flask, request, redirect, url_for
import os

app = Flask(__name__)

@app.route('/', methods=['GET', 'POST'])
def upload_file():
    if request.method == 'POST':
        # Vérifie que le champ de fichier a été envoyé avec le formulaire
        if 'file' not in request.files:
            return redirect(request.url)

        file = request.files['file']

        # Vérifie que l'utilisateur a sélectionné un fichier
        if file.filename == '':
            return redirect(request.url)

        # Enregistre le fichier sur le serveur
        if file:
            filename = file.filename
            file.save(os.path.join(app.config['UPLOAD_FOLDER'], filename))
            return 'Fichier envoyé avec succès !'

    # Affiche le formulaire d'envoi de fichier
    return '''
        <form method="post" enctype="multipart/form-data">
            <input type="file" name="file">
            <input type="submit" value="Envoyer">
        </form>
    '''

if __name__ == '__main__':
    app.config['UPLOAD_FOLDER'] = '/chemin/vers/dossier/upload' # Chemin vers le dossier où les fichiers seront enregistrés
    app.run(debug=True)