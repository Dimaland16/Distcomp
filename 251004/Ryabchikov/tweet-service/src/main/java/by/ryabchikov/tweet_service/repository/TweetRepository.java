package by.ryabchikov.tweet_service.repository;

import by.ryabchikov.tweet_service.entity.Tweet;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.Optional;

@Repository
public interface TweetRepository extends JpaRepository<Tweet, Long> {
    Optional<Tweet> findByTitle(String title);
}