# Covert Channel Using the TLS Random Field

[![Quality gate status](https://sonarcloud.io/api/project_badges/measure?project=joelvaz0x01_tls-covert-channel-code&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=joelvaz0x01_tls-covert-channel-code)
[![Duplicated Lines (%)](https://sonarcloud.io/api/project_badges/measure?project=joelvaz0x01_tls-covert-channel-code&metric=duplicated_lines_density)](https://sonarcloud.io/summary/new_code?id=joelvaz0x01_tls-covert-channel-code)
[![Lines of Code](https://sonarcloud.io/api/project_badges/measure?project=joelvaz0x01_tls-covert-channel-code&metric=ncloc)](https://sonarcloud.io/summary/new_code?id=joelvaz0x01_tls-covert-channel-code)

MSc thesis developed at [UA - Portugal](https://www.ua.pt/) for the Cybersecurity course.

> [!WARNING]
> This code is solely for educational purposes.
>
> This work was deliberately not fully developed to prevent malicious use in a real-world scenario but contains enough information to demonstrate a real-world attack.
>
> All tests were performed under a controlled environment; no real servers were involved.

## Abstract

This thesis code builds a covert channel in the initial handshake messages of the TLS protocol that can be applied to ClientHello and ServerHello. The goal is to utilize the 32-byte random field present in these messages to transmit files or information while maintaining the apparent randomness of that field. Due to the small size of that field, the transmission of files or information must be performed over a limited bandwidth. To ensure the file or information transmission efficiency, Fountain Codes will be used to encode the data before it is transmitted. The integrity of the data or information of the Fountain Codes will be guaranteed by a hash that will be transmitted simultaneously with the encoded data or information.
