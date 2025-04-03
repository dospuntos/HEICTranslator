# HEIC image translator for Haiku

This is a Haiku image translator for HEIC images, allowing them to be opened in Haiku applications that support the Translation Kit.

- This is a work in progress and has not been thoroughly tested.
- Currently, it only supports reading HEIC files, not writing them.
- Based on the code from https\://discuss.haiku-os.org/t/heic-image-format-actually-easy-to-parse-with-libheif/14883.
- HEIC images for testing can be found at https://github.com/tigranbs/test-heic-images

## Installation

### Clone the repository

To get the source code, run:

```sh
git clone https://github.com/dospuntos/HEICTranslator.git
cd HEICTranslator
```

### Build the Translator

Compile the translator using `make`:

```sh
make
```

### Install the Translator

To install the translator in the Haiku system:

```sh
make install
```

This copies the `HEICTranslator` binary to `~/config/non-packaged/add-ons/Translators`.

## Usage

Once installed, applications that use the Translation Kit (such as ShowImage) should automatically detect and open HEIC images.

## Uninstallation

To remove the translator:

```sh
rm ~/config/non-packaged/add-ons/Translators/HEICTranslator
```

## License

This project is licensed under the MIT License.
