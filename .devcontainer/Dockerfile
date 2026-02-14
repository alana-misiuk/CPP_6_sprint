FROM ubuntu@sha256:79efa276fdefa2ee3911db29b0608f8c0561c347ec3f4d4139980d43b168d991

# Update the package database and install required tools
RUN apt clean
RUN apt update

# Install common tools
RUN apt install -y python3-minimal python3-pip pipx git less vim sudo cmake make g++-15 clang-format clangd-20

# Install additional tools and libraries
RUN apt install -y libgtest-dev

# Cleanup
RUN apt autoremove -y \
    && apt autoclean \
    && rm -rf /var/lib/apt/lists/* \
    && rm -rf /var/cache/apt/* \
    && rm -rf /tmp/*

# Update  gcc/g++ version to the latest ones
RUN ln -fs /usr/bin/g++-15 /usr/bin/g++
RUN ln -fs /usr/bin/gcc-15 /usr/bin/gcc

# Create a non-root user 'dev'
RUN useradd -ms /bin/bash dev \
    && echo "dev ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers.d/dev \
    && chmod 0440 /etc/sudoers.d/dev

# Switch to the 'dev' user
USER dev

RUN git config --global core.editor code

# Install conan through pipx
RUN pipx install conan && \
    echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
RUN pipx ensurepath

ENV PATH="/root/.local/bin:${PATH}"

# Copy conan profile

RUN mkdir -p /home/dev/.conan2/profiles/default
# COPY default /home/dev/.conan2/profiles/default
RUN sudo chown -R dev:dev /home/dev/.conan2
RUN sudo chmod 666 /home/dev/.conan2/profiles/default


# Set the default command
CMD ["/bin/bash"]
